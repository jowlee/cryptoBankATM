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

typedef struct {
  int   value;
  char  string[128];
} thread_parm_t;


//HashMap of usernames to a tuple of user info or user object containing {username, pin, balance}

void deposit(const std::string* user, unsigned long amount);
void balance(const std::string* user);

void *ioThread(void *threadid);

void error(const char *msg);
void *socketThread(int* sock);

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
    // pthread_t* threadNum =;
    while (1) {

        newsockfd = accept(sockfd,
              (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
            error("ERROR on accept");
        pthread_t thread;
        int        *parm=NULL;
        // pid = fork();
        // pthread_create(NULL, NULL, socketThread, (void *)newsockfd);
        // pthread_create(NULL, NULL, socketThread, &newsockfd);
        // pthread_create(NULL, NULL, socketThread, (void *) (intptr_t) newsockfd);
        pthread_create(&thread, NULL, socketThread, NULL);
        // if (pid < 0)
        //     error("ERROR on fork");
        // if (pid == 0)  {
        //     close(sockfd);
        //     dostuff(newsockfd);
        //     exit(0);
        // }
        // else close(newsockfd);
        close(newsockfd);
        // int threadNum* ++;
    } /* end of while */
    close(newsockfd);
    close(sockfd);
  	//create 2 threads, one for I/O another for incomming connections
  	return 0;
}

//
void deposit(const std::string* user, unsigned long amount) {

}

void balance(const std::string* user) {

}
void error(const char *msg)
{
    std::cerr << msg <<std::endl;
    exit(1);
}


void *ioThread(void *threadid) {
	// Read in commands from cin
	for(std::string line; getline(std::cin, line);) {
		int index = 0;
		std::string command = "";

		// Read input until we read a space, then for each character add it to the command string
		for(; line[index] != ' ' || index >= line.length(); index++) {
			command += line[index];
		}

		// Skip any number of spaces
		for(; line[index] == ' ' || index >= line.length(); index++);

		std::string username = "";

		// Read the next chars and put into username
		for(; line[index] != ' '  || index >= line.length(); index++) {
			username += line[index];
		}
		//deposit ​[username] [amount] ­ Increase <username>’s ​balance by <amount>
		if(command.compare(0,8,"deposit")) {
			std::string amount = "";

			// Skip any number of spaces
			for(; line[index] == ' ' || index >= line.length(); index++);

			// Read until the next space and put it in amount
			for(; line[index] != ' ' || index >= line.length(); index++) {
				amount += line[index];
			}

			// Execute deposit command
			deposit(&username, atol(amount.c_str()));
		}
		//balance​ [username] ­ Print the balance of <username>
		else if(command.compare(0,8,"balance")) {

			// Execute balance command
			balance(&username);
		}
	}
	pthread_exit(NULL);		// Ends the thread
}

void* socketThread(void * sockid) {
  // int sock = (intptr_t) sockid;
  // int sock = (int)sockid;
  int sock = 5;
  int n;
  char buffer[256];

  bzero(buffer,256);
  n = read(sock,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  printf("Here is the message: %s\n",buffer);
  n = write(sock,"I got your message",18);
  if (n < 0) error("ERROR writing to socket");
	pthread_exit(NULL);		// Ends the thread
  return NULL;
}
