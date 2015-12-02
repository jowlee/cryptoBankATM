/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <cstring>
#include <pthread.h>			// For multithreading
#include <stdint.h>
#include <sstream>
#include <iomanip>
#include "crypto.h"

// #define write cwrite
// #define read cread

#include "serverCommands.cpp"



struct thread_info { 	   		/* Used as argument to thread_start() */
	pthread_t 	thread_id;     /* ID returned by pthread_create() */
	int 			sock;			/* Socket. I think */
	userDB		*users; 		/* Pointer to global users object */
	//session_type	*sessions;		// pointer to global sessions object
	// CryptoPP::SecByteBlock session_key;
	// string 			username;
};


//HashMap of usernames to a tuple of user info or user object containing {username, pin, balance}

std::string advanceCommand(const std::string& line, int &index);
void advanceSpaces(const std::string &line, int &index);

void error(const char *msg);

void* consoleThread(void *threadid);
void* socketThread(void* arg);

int main(int argc, char *argv[]) {
  	if (argc != 2) {                     // Test for correct number of arguments
  		std::cerr << "Usage: " << argv[0] << " <Server Port>" << std::endl;
  		exit(1);
  	}
    // socket initialization
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        std::cerr << "Usage: ATMserver <Port>" << std::endl;
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    // assign socket to port number
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    pthread_t thread;
    thread_info args;

    userDB* users = new userDB();
    init_bank(users);
    args.users = users;

    pthread_create(&thread, NULL, consoleThread, &args);

    while (1) {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) {
          error("ERROR on accept");
          continue;
        }
        pthread_t thread;
        // thread_info args;
        args.sock = newsockfd;

        // pid = fork();
        pthread_create(&thread, NULL, socketThread, &args);
        // close(newsockfd);
        // int threadNum* ++;
    } /* end of while */
    close(newsockfd);
    close(sockfd);
  	//create 2 threads, one for I/O another for incomming connections
  	return 0;
}

//
void error(const char *msg) {
    std::cerr << msg <<std::endl;
    // exit(1);
}
// Read input until we read a space, then for each character add it to the command string
std::string advanceCommand(const std::string& line, int &index) {
  std::string command = "";
  if (index >= line.length()) return command;
  for(; line[index] != ' ' && line[index] != '\0' && index < line.length(); index++) {
    command += line[index];
  }
  if (index != line.length()) {
    advanceSpaces(line, index);
  }
  return command;
}
// Skip any number of spaces
void advanceSpaces(const std::string &line, int &index) {
  for(; line[index] == ' ' && index < line.length(); index++);
}

std::string genSessionKey(std::string username) {
  int secret = rand() % 1000 + 1;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(4) << secret;
  return username + '_' + ss.str();
}
std::string parseCommands(char buffer[], userDB* users, std::string& sessionKey, int& sentNumber) {
  int index = 0;
  bool loggedIn = false;
  std::string input(buffer);
  userInfo *thisUser;
  std::string sendStr;

  int cliNumber = atoi(advanceCommand(input, index).c_str());
  sentNumber ++;
  if (cliNumber != sentNumber) {
    return "not the message I was expecting";
  }
  sentNumber ++;


  if (sessionKey.length() != 0) {
    std::string cliSeshKey = advanceCommand(input, index);

    int pos = sessionKey.find('_');
    std::string someUser = sessionKey.substr(0, pos);
    thisUser = users->findUser(someUser);
    if (thisUser == NULL) {
      return "session Key has been tampered with";
    }
    else {
      loggedIn = thisUser->isLoggedIn();

    }


    // checks if user is trying to logout
    if (cliSeshKey.compare("logout") == 0 && loggedIn) {
      sessionKey = "";
      // std::cout << "atm connection ~ : " <<  "user logged out" << std::endl;
      thisUser->logout();
      return "logged_out";
    }
    // checks to see if client's session key equals clients session key
    if (sessionKey.compare(cliSeshKey) != 0) return "session key has been tampered with";
  }
  // std::cout << "session key: " << sessionKey << std::endl;

  std::string command = advanceCommand(input, index);
  // std::cout << "balance: " << (command.compare("balance") == 0) << " " << loggedIn << std::endl;
  if (command.compare("login") == 0) {
    std::string username = advanceCommand(input, index);
    std::string pin = advanceCommand(input, index);
    if (users->loginUser(username, pin)) {
      // std::cout << username << " logged in!" << std::endl;
      sessionKey = genSessionKey(username);
      sendStr = "y " + sessionKey;
    }
    else {
      sendStr = "n";
    }
  }
  else if (command.compare("balance") == 0 && loggedIn) {
    sendStr = thisUser->getBalance();
  }
  else if (command.compare("withdraw") == 0 && loggedIn) {
    std::string amount = advanceCommand(input, index);
    std::string takenOut = thisUser->withdraw(amount);
    sendStr = "withdrew";
  }
  else if (command.compare("transfer") == 0 && loggedIn) {
    std::string amount = advanceCommand(input, index);
    std::string sendingTo = advanceCommand(input, index);
    users->transfer(thisUser->getName(), sendingTo, amount);
    sendStr = "transfered";
  }
  else if (command.compare("check") == 0 && loggedIn) {
    std::string name = advanceCommand(input, index);
    bool exists = users->userExists(name);
    if (exists) sendStr = "y";
    else sendStr = "n";
  }
  else if (command.compare("init") == 0) {
    sendStr = "connected to bank";
  }
  else {
    std::cout << "bad command" << std::endl;
    sendStr = "error! bad command!";
  }
  std::stringstream ss;
  ss << sentNumber;
  return ss.str() + " " + sendStr;
}

void* socketThread(void* args) {
  std::cout << std::endl;
  struct thread_info *tinfo;
  tinfo = (thread_info *) args;
  int sock = tinfo->sock;
  userDB *users = tinfo->users;
  std::string sessionKey;
  int sentNumber = 0;

  bool logginIn = false;

  while (1) {
    int n;
    char buffer[256];
    char sendBuffer[256];
    bzero(buffer,256);
    bzero(sendBuffer,256);

    n = read(sock,buffer,255);

    if (n < 0) {
      error("ERROR writing to socket");
      close(sock);
      break;
    }
    if (n == 0) {
      std::cout << "atm connection ~ : " <<  "socket# " << sock << " disconnected" << std::endl;
      if (sessionKey.length() != 0) {
        int pos = sessionKey.find('_');
        userInfo *thisUser;
        std::string someUser = sessionKey.substr(0, pos);
        thisUser = users->findUser(someUser);
        thisUser->logout();
      }
      break;
    }
    if (std::string(buffer).compare("init") == 0) {
      std::cout << "atm connection ~ : ";
      std::cout << "socket# " << sock << " connected" << std::endl;
    } else {
      std::cout << "atm connection ~ : ";
      std::cout << buffer << std::endl;
    }
    std::string send = parseCommands(buffer, users, sessionKey, sentNumber);
    n = write(sock, send.c_str(), send.length());
    if (send.compare("not the message I was expecting") == 0) {
      close(sock);
      break;
    }
  }
  pthread_exit(NULL);		// Ends the thread
  return NULL;
}

void *consoleThread(void *args) {
  struct thread_info *tinfo;
  tinfo = (thread_info *) args;
  userDB *users = tinfo->users;
  // string that is printed to the console
  std::string sendStr;

  std::cout << ("bank ~ : ");
	for(std::string line; getline(std::cin, line);) {

		int index = 0;
		std::string command = advanceCommand(line, index);
		std::string username = advanceCommand(line, index);
    if (command.compare("balance") != 0 && command.compare("deposit") != 0) {
      sendStr = "Bank Commands: [deposit|balance] [username] <amount>";
    }
    else if (!users->userExists(username)) {
      sendStr = "user \"" + username + "\" does not exist";
    }
		else if(command.compare("deposit") == 0) {
		  //deposit ​[username] [amount] ­ Increase <username>’s ​balance by <amount>
			std::string amount = advanceCommand(line, index);
      // std::string test = "test";
      int isNum = atoi(amount.c_str());
      if (isNum < 0) {
        sendStr = "no negative deposits!!!";
      } else {
			  sendStr = "deposited " + amount;
        deposit(username, amount, users);
      }
		}
		else if(command.compare("balance") == 0) {
		  //balance​ [username] ­ Print the balance of <username>
			sendStr = balance(username, users);
		}
    std::cout << sendStr << std::endl;
    std::cout << ("bank ~ : ");
  }
	pthread_exit(NULL);		// Ends the thread
}
