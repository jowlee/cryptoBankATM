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

//HashMap of usernames to a tuple of user info or user object containing {username, pin, balance}

void deposit(const String* user, unsigned long amount);
void balance(const String* user);

void *ioThread(void *threadid);
void *socketThread(void *threadid);

int main(int argc, char *argv[]) {
	if (argc != 2) {                     // Test for correct number of arguments
		cerr << "Usage: " << argv[0] << " <Server Port>" << endl;
		exit(1);
	}

	//create 2 threads, one for I/O another for incomming connections
	return 0;
}

//
void deposit(const String* user, unsigned long amount) {

}

void balance(const String* user) {

}
void error(const char *msg)
{
    std::cerr << msg <<std::endl;
    exit(1);
}


void *ioThread(void *threadid) {
	// Read in commands from cin
	for(String line; getline(cin, line);) {
		int index = 0;
		String command = "";

		// Read input until we read a space, then for each character add it to the command string
		for(; line[index] != " " || index >= line.length(); index++) {
			command += line[index];
		}

		// Skip any number of spaces
		for(; line[index] == " " || index >= line.length(); index++);

		String username = "";

		// Read the next chars and put into username
		for(; line[index] != " " || index >= line.length(); index++) {
			username += line[index];
		}
		//deposit ​[username] [amount] ­ Increase <username>’s ​balance by <amount>
		if(command.compare(0,8,"deposit")) {
			String amount = "";

			// Skip any number of spaces
			for(; line[index] == " " || index >= line.length(); index++);

			// Read until the next space and put it in amount
			for(; line[index] != " " || index >= line.length(); index++) {
				amount += line[index];
			}

			// Execute deposit command
			deposit(&username, stol(amount, nullptr))
		}
		//balance​ [username] ­ Print the balance of <username>
		else if(command.compare(0,8,"balance")) {

			// Execute balance command
			balance(&username);
		}
	}
	pthread_exit(NULL);		// Ends the thread
}
int recvMessage(int newsockfd, char  buffer[256], int length) {
  int n;
  n = read(newsockfd,buffer,length);
  if (n < 0) {
    error("ERROR reading from socket");
  }
  else if (n == 0) {
    // no message
  }
  else {
    std::cout << "Here is the message: " << buffer << std::endl;
  }
}
int sendMessage(int newsockfd, std::string message) {
  int n;
  int length  = message.length();
  const char *cstr = message.c_str();

  n = write(newsockfd, cstr, length);
  if (n < 0) {
    error("ERROR writing to socket");
  }
}

void *socketThread(void *threadid){
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
  // listens for client connections
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  // blocking wait for the connection
  newsockfd = accept(sockfd,
             (struct sockaddr *) &cli_addr,
             &clilen);
  if (newsockfd < 0) {
      error("ERROR on accept");
  }

  bzero(buffer,256);
  recvMessage(newsockfd, buffer, 255);
  sendMessage(newsockfd, "I got your message");

  close(newsockfd);
  close(sockfd);
	pthread_exit(NULL);		// Ends the thread
}
