#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "clientCommands.cpp"

void closeSocket(int atmSocket) {
	printf("Closing \n");
	fflush(stdout);
	close(atmSocket);
	exit(1);
}

int main(int argc, char *argv[]){

  // Check for arguments (port number is provided)
  if(argc != 2){
		printf("Run ./ATMClient <proxy-port-number>\n");
		return -1;
	}

  //socket setup
	unsigned int proxyPortNo = atoi(argv[1]);
	int atmSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(atmSocket == -1){
		printf("Did not connect to socket\n");
		return -1;
	}

	//Close sock on Ctrl-C
	signal(SIGINT, closeSocket);

  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(proxyPortNo);
  addr.sin_addr.s_addr = INADDR_ANY;

  if(0 != connect(atmSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr))){
    printf("Did not connect to socket\n");
    return -1;
  }

	// Initial message
	int n = write(atmSocket, "init", 4);
	if (n < 0) error("ERROR something is wrong");

  //input loop
  bool loggedin = false;
	std::string sessionKey;

  while(1){
    printf("atm ~ : ");

    char buf[80];
    fgets(buf, 79, stdin);
		buf[strlen(buf)-1] = '\0';	//trim off trailing newline
    int index = 0;
    std::string input(buf);
    std::string command = advanceCommand(input, index);
    // std::cout << command << std::endl;


    // Wait for user to input command
    // int n = write(atmSocket, "123123", 16);
    //
    // char buffer[256];
    // bzero(buffer,256);
    // printf("Here is the message: %s\n",buffer);

    // n = read(sock,buffer,255);
    // if (n < 0) error("ERROR reading from socket");
    // std::cout << "compare exit: " <<  command.length() << std::endl;
    // std::cout << strlen(command.c_str()) << std::endl;

    // std::cout << command << " " << command.compare("exit") << std::endl;
    if(command.compare("exit") == 0){
      closeSocket(atmSocket);
    }

    if(loggedin){
      // Login in
      if(command.compare("balance") == 0){
        advanceSpaces(input, index);
        std::cout << "Obtaining Balance..." << std::endl;
        balance("test", atmSocket);
      }

      else if(command.compare("withdraw") == 0){
        advanceSpaces(input, index);
        std::string amountString = advanceCommand(input, index);
        float amount = atof(amountString.c_str());
        std::cout << amount << std::endl;
      }

      else if(command.compare("transfer") == 0){
        advanceSpaces(input, index);
        std::string amountString = advanceCommand(input, index);
        float amount = atof(amountString.c_str());
        advanceSpaces(input, index);
        std::string username = advanceCommand(input, index);
        std::cout << username << std::endl;
      }

      else if(command.compare("logout") == 0){
        advanceSpaces(input, index);
        std::cout << "Logging Out Now..." << std::endl;
        std::cout << "GoodBye" << std::endl;
        exit(0);
      }
      else{
        std::cout << "You suck man" << std::endl;
      }
    }

    else{
      // Login in
      if(command.compare("login") == 0){
        advanceSpaces(input, index);
        std::string username = advanceCommand(input, index);
        std::cout << username << std::endl;
        std::string ans = login(username, atmSocket);
				if(ans != "broken"){
					sessionKey = ans;
					loggedin = true;
				}
      }
    }
  }

  //cleanup
  close(atmSocket);
  return 0;
  }
