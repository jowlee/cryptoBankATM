#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>


void closeSocket(int atmSocket) {
	printf("Closing \n");
	fflush(stdout);
	close(atmSocket);
	exit(1);
}

void error(const char *msg){
    perror(msg);
    exit(0);
}
// Validate that pin is 6 characters long
bool validPin(std::string pin){
  if(pin.length() == 6){
    return true;
  }
  return false;
}

bool checkGood(std::string response){
  if(response.compare("error! bad command!") == 0){
    std::cout << "didn't work" << std::endl;
    return true;
  }
  if(response.compare("-1") == 0){
    std::cout << "Error" << std::endl;
    return true;
  }
  else return false;
}

// Read input until we read a space, then for each character add it to the command string
std::string advanceCommand(const std::string& line, int &index) {
  std::string command = "";
  for(; line[index] != '\n' && line[index] != '\0' && line[index] != ' ' && index <= line.length(); index++) {
    command += line[index];
  }
  return command;
}
// Skip any number of spaces
void advanceSpaces(const std::string &line, int &index) {
  for(; line[index] == ' ' && index <= line.length(); index++);
}



int parseRecieve(char * recieve, int messageNumber, int socketNo){
  int messageIndex = 0;
  std::string number = advanceCommand(recieve, messageIndex);
  int num = atoi(number.c_str());

  std::cout << recieve << std::endl;

  if(num == -1){
    // Error dont read response
    bzero(recieve,256);
    closeSocket(socketNo);
    exit(-1);
    // return messageNumber;
  }
  else if(num!= (++messageNumber)){
    // Error dont read response
    bzero(recieve,256);
    closeSocket(socketNo);
    exit(-1);
    // return messageNumber;
  }

  std::string converted(recieve);
  std::string subConverted = converted.substr(messageIndex, converted.length());
  bzero(recieve,256);
  strcpy (recieve,subConverted.c_str());
  messageNumber++;

  std::cout << recieve << std::endl;

  return messageNumber;
}

int sendRecieve(int socketNo, char sendMessage[], char*  recieve, int messageNumber){
  std::stringstream ss;
  ss << messageNumber;
  std::string num = ss.str();
  char numMessage[256];
  strcpy (numMessage,num.c_str());
  strcat(numMessage, " ");
  strcat(numMessage, sendMessage);

  int n = write(socketNo, numMessage, strlen(numMessage)+1);
  if (n < 0) error("ERROR writing to socket");

  bzero(recieve,256);
  n = read(socketNo,recieve,255);
  if (n < 0) error("ERROR reading from socket");
  messageNumber = parseRecieve(recieve, messageNumber, socketNo);
  return messageNumber;
}

int login(const std::string username, int socketNo, int messageNumber, char *ans){

  std::string cardInfo;
  char cardName[80];
  strcpy (cardName,username.c_str());
  strcat(cardName, ".card");

  std::ifstream inFile( cardName);

  std::string str;
  if(! inFile){
    std::cout << "Broken... Couldn't open file" << std::endl;
    return messageNumber;
  }

  if(!std::getline(inFile, str)){
    std::cout << "Broken..." << std::endl;
    return messageNumber;
  }

  std::string password;
  std::cout << "Password : ";
  std::cin >> password;

  if(!validPin(password)){
    std::cout << "Error Incorrect Password" << std::endl;
    return messageNumber;
  }

  char message[256];

  strcpy(message, "login ");
  strcat(message, username.c_str());
  strcat(message, " ");
  strcat(message, password.c_str());

  char buffer[256];
  messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
  if(checkGood(std::string(buffer))){
    return messageNumber;
  }

  std::string response(buffer);
  int index = 0;
  std::string works = advanceCommand(response, index);

  if(works.compare("y") == 0){
    advanceSpaces(response, index);
    std::string code = advanceCommand(response, index);
    std:: cout << "Logged in" << std::endl;
    strcpy (ans, code.c_str());

  }
  else{
    std:: cout << "Didn't work .... sorry" << std::endl;
    strcpy (ans, "broken");
  }
  return messageNumber;
}

int balance(const std::string sessionKey, int socketNo, int messageNumber){

  char message[256];
  strcpy (message, sessionKey.c_str());
  strcat(message, " balance");

  char buffer[256];
  messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
  if(checkGood(std::string(buffer))){
    return messageNumber;
  }
  std::cout << "balance is " << buffer << std::endl;
  return messageNumber;

}

int withdraw(const std::string sessionKey, std::string amount, int socketNo, int messageNumber){

  // Check balance for sufficient funds
  char message[256];
  strcpy (message, sessionKey.c_str());
  strcat(message, " balance");

  char buffer[256];
  messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
  if(checkGood(std::string(buffer))){
    return messageNumber;
  }

  float balance = atof(buffer);
  float amountf = atof(amount.c_str());

  // If sufficient funds, allow withdraw
  if(amountf <= balance){
    bzero(message,256);
    strcpy (message, sessionKey.c_str());
    strcat (message," withdraw ");
    strcat(message, amount.c_str());

    bzero(buffer,256);
    messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
    if(checkGood(std::string(buffer))){
      return messageNumber;
    }
    std:: cout << buffer << std::endl;
  }
  else{
    std::cout << "Insufficient funds to transfer" << buffer << std::endl;
  }
  return messageNumber;
}

int transfer(const std::string sessionKey, std::string amount, std::string username, int socketNo, int messageNumber){

  // Check balance for sufficient funds
  // Check balance for sufficient funds
  char message[256];
  strcpy (message, sessionKey.c_str());
  strcat(message, " balance");

  char buffer[256];
  bzero(buffer,256);
  messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
  if(checkGood(std::string(buffer))){
    return messageNumber;
  }

  float balance = atof(buffer);
  float amountf = atof(amount.c_str());

  // If sufficient funds, allow transfer
  if(amountf <= balance){

    // Check if other user exitss
    bzero(message,256);
    strcpy (message, sessionKey.c_str());
    strcat (message," check ");
    strcat(message, username.c_str());

    bzero(buffer,256);
    messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
    if(checkGood(std::string(buffer))){
      return messageNumber;
    }

    std:: cout << "Response is  " << buffer << std::endl;

    if((std::string(buffer)).compare("y") == 0){
      bzero(message,256);
      strcpy (message, sessionKey.c_str());
      strcat (message," transfer ");
      strcat(message, amount.c_str());
      strcat(message, " ");
      strcat(message, username.c_str());

      bzero(buffer,256);
      messageNumber = sendRecieve(socketNo, message, buffer, messageNumber);
      if(checkGood(std::string(buffer))){
        return messageNumber;
      }

      std:: cout << buffer << std::endl;
    }
    else{
      std:: cout << "The other user does not exist" << std::endl;
    }
  }

  else{
    std::cout << "Insufficient funds to transfer" << buffer << std::endl;
  }
  return messageNumber;
}
