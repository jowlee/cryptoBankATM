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

	char mess[256];
	strcpy(mess,"nOsEsSiOnKeY ");
	strcat(mess, "init");
	int n = write(atmSocket, mess, 4);
	if (n < 0) error("ERROR something is wrong");
	char buf[256];
	n = read(atmSocket,buf,255);
  if (n < 0) error("ERROR reading from socket");
  //input loop
  bool loggedin = false;
	std::string sessionKey;
	std::string input;

	printf("atm ~ : ");

  while(1){

		// std::cout << std::endl;
		getline(std::cin, input);

    // fgets(buf, 255, stdin);
		// buf[strlen(buf)-1] = '\0';	//trim off trailing newline
    // std::string input(buf);
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
        balance(sessionKey, atmSocket);
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
				closeSocket(atmSocket);
        std::cout << "Logging Out..." << std::endl;
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

		printf("atm ~ : ");

  }

  //cleanup
  close(atmSocket);
  return 0;
  }
