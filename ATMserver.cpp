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
    exit(1);
}
// Read input until we read a space, then for each character add it to the command string
std::string advanceCommand(const std::string& line, int &index) {
  std::string command = "";
  for(; line[index] != ' ' && line[index] != '\0' && index < line.length(); index++) {
    command += line[index];
  }
  return command;
}
// Skip any number of spaces
void advanceSpaces(const std::string &line, int &index) {
  for(; line[index] == ' ' && index < line.length(); index++);
}

void *consoleThread(void *threadid) {
	// Read in commands from cin
  std::cout << ("bank ~ : ");
	for(std::string line; getline(std::cin, line);) {

		int index = 0;
		std::string command = advanceCommand(line, index);
    advanceSpaces(line, index);
		std::string username = advanceCommand(line, index);

		//deposit ​[username] [amount] ­ Increase <username>’s ​balance by <amount>
		if(command.compare("deposit") == 0) {
      advanceSpaces(line, index);
			std::string amount = advanceCommand(line, index);

			// Execute deposit command
			deposit(&username, atol(amount.c_str()));
		}
		//balance​ [username] ­ Print the balance of <username>
		else if(command.compare("balance") == 0) {
			// Execute balance command
			balance(&username);
		}
    std::cout << ("bank ~ : ");

	}
	pthread_exit(NULL);		// Ends the thread
}
std::string genSessionKey(std::string username) {
  int secret = rand() % 1000 + 1;
  std::stringstream ss;
  ss << std::setfill('0') << std::setw(4) << secret;
  return username + '_' + ss.str();
}
std::string parseCommands(char buffer[], userDB* users, std::string& sessionKey) {
  int index = 0;
  bool loggedIn = false;
  std::string input(buffer);
  userInfo *thisUser;
  std::string sendStr;

  if (sessionKey.length() != 0) {
    std::cout << "session key: " << sessionKey << std::endl;
    // should check to see if client's session key equals clients session key
    std::string seshKey = advanceCommand(input, index);
    advanceSpaces(input, index);
    int pos = sessionKey.find('_');
    std::string someUser = sessionKey.substr(0, pos);
    thisUser = users->findUser(someUser);
    loggedIn = true;
  }

  std::string command = advanceCommand(input, index);
  // std::cout << "balance: " << (command.compare("balance") == 0) << " " << loggedIn << std::endl;

  advanceSpaces(input, index);
  if (command.compare("login") == 0) {
    std::string username = advanceCommand(input, index);
    advanceSpaces(input, index);
    std::string pin = advanceCommand(input, index);
    if (users->loginUser(username, pin)) {
      std::cout << username << " logged in!" << std::endl;
      sessionKey = genSessionKey(username);
      sendStr = "y " + sessionKey;
    } else {
      sendStr = "n";
    }
  } else if (command.compare("balance") == 0 && loggedIn) {
    sendStr = thisUser->getBalance();
  } else if (command.compare("withdraw") == 0 && loggedIn) {
    std::string amount = advanceCommand(input, index);
    std::string takenOut = thisUser->withdraw(amount);
    sendStr = "withdrew";
  } else if (command.compare("transfer") == 0 && loggedIn) {
    
  } else if (command.compare("logout") == 0 && loggedIn) {

  } else if (command.compare("init") == 0) {
    sendStr = "connected to bank";
  } else {
    std::cout << "bad command" << std::endl;
    sendStr = "error! bad command!";
  }
  return sendStr;
}

void* socketThread(void* args) {
  std::cout << std::endl;
  // std::cout << "atm connection ~ : " << "connection made" << std::endl;
  struct thread_info *tinfo;
  tinfo = (thread_info *) args;
  int sock = tinfo->sock;
  userDB *users = tinfo->users;
  std::string sessionKey;

  bool logginIn = false;

  while (1) {
    int n;
    char buffer[256];
    char sendBuffer[256];
    bzero(buffer,256);
    bzero(sendBuffer,256);

    n = read(sock,buffer,255);
    // buffer[n-1] = '\0';
    // printf("Here is the message: %s\n",buffer);

    if (n < 0) error("ERROR writing to socket");
    if (buffer == "exit" || n == 0) {
      std::cout << "atm connection ~ : " <<  "exiting!" << std::endl;
	    pthread_exit(NULL);		// Ends the thread
      break;
    }

    std::cout << "atm connection ~ : ";
    std::cout << buffer << std::endl;
    std::string send = parseCommands(buffer, users, sessionKey);
    n = write(sock, send.c_str(), send.length());
  }
  return NULL;
}
