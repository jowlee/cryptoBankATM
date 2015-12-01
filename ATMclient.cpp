#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "clientCommands.cpp"

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

	int messageNumber = 1;

	char message[256];
	strcpy(message,  "init");

	char buffer[256];
	messageNumber = sendRecieve(atmSocket, message, buffer, messageNumber);

  //input loop
  bool loggedin = false;
	std::string sessionKey;
	std::string input;

	printf("atm ~ : ");

  while(1){

		getline(std::cin, input);
    int index = 0;
		if (input.length() == 0) continue;
    std::string command = advanceCommand(input, index);

    if(command.compare("exit") == 0){
      closeSocket(atmSocket);
    }

    if(loggedin){
      // Login in
      if(command.compare("balance") == 0){
        advanceSpaces(input, index);
        std::cout << "Obtaining Balance..." << std::endl;
        messageNumber = balance(sessionKey, atmSocket, messageNumber);
      }

      else if(command.compare("withdraw") == 0){
        advanceSpaces(input, index);
        std::string amountString = advanceCommand(input, index);
        float amount = atof(amountString.c_str());
				messageNumber = withdraw(sessionKey, amountString, atmSocket, messageNumber);
      }

      else if(command.compare("transfer") == 0){
        advanceSpaces(input, index);
        std::string amountString = advanceCommand(input, index);
        float amount = atof(amountString.c_str());
        advanceSpaces(input, index);
        std::string username = advanceCommand(input, index);
				messageNumber = transfer(sessionKey, amountString, username, atmSocket, messageNumber);
      }

      else if(command.compare("logout") == 0){
        advanceSpaces(input, index);
				loggedin = false;
        sessionKey = "";

				bzero(buffer,256);
				bzero(message,256);

				strcpy(message,  "logout");
				messageNumber = sendRecieve(atmSocket, message, buffer, messageNumber);

        std::cout << "Logging Out..." << std::endl;

      }
      else{
        std::cout << "Command not found" << std::endl;
      }
    }

    else{
      // Login in
      if(command.compare("login") == 0){
        advanceSpaces(input, index);
        std::string username = advanceCommand(input, index);
				char ans[256];
				messageNumber = login(username, atmSocket, messageNumber, ans);

				if(ans != "broken"){
					sessionKey = ans;
					loggedin = true;
				}
      }
    }

		printf("atm ~ : ");

  }

  //cleanup
  close(atmSocket);
  return 0;
  }
