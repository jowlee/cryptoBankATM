#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>

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

// Read input until we read a space, then for each character add it to the command string
std::string advanceCommand(const std::string& line, int &index) {
  std::string command = "";
  for(; line[index] != '\n' && line[index] != ' ' && index <= line.length(); index++) {
    command += line[index];
  }
  return command;
}
// Skip any number of spaces
void advanceSpaces(const std::string &line, int &index) {
  for(; line[index] == ' ' && index <= line.length(); index++);
}

std::string login(const std::string username, int socketNo){

  std::string cardInfo;
  char cardName[80];
  strcpy (cardName,username.c_str());
  strcat(cardName, ".card");

  std::ifstream inFile( cardName);
  std::cout << "Opening " << cardName << std::endl;

  std::string str;
  if(! inFile){
    std::cout << "Broken... Couldn't open file" << std::endl;
  }

  if(!std::getline(inFile, str)){
    std::cout << "Broken..." << std::endl;
  }

  std::string password;

  std::cout << "Password : ";
  std::cin >> password;
  std::cout << password << std::endl;

  if(!validPin(password)){
    std::cout << "Error Incorrect Password" << std::endl;
  }

  char message[256];

  strcpy (message,"login ");
  strcat(message, username.c_str());
  strcat(message, " ");
  strcat(message, password.c_str());

  int n = write(socketNo, message, strlen(message));
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  std::cout << buffer << std::endl;

  std::string response(buffer);
  int index = 0;
  std::string works = advanceCommand(response, index);

  std::cout << "works is " << works.length() << works << std::endl;

  if(works.compare("y") == 0){
    advanceSpaces(response, index);
    std::string code = advanceCommand(response, index);
    std::cout << "code " << code << std::endl;

    return code;
  }
  else{
    std:: cout << "Didn't work .... sorry" << std::endl;
    return "broken";
  }

}

void balance(const std::string sessionKey, int socketNo){
  int n = write(socketNo, "balance", 16);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  std:: cout << buffer << std::endl;
}

void withdraw(const std::string sessionKey, std::string amount, int socketNo){

  // Check balance for sufficient funds
  int n = write(socketNo, "balance", 16);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  std:: cout << "balance is " << buffer << std::endl;

  float balance = atof(buffer);
  float amountf = atof(amount.c_str());

  // If sufficient funds, allow withdraw
  if(amountf < balance){
    char message[256];
    strcpy (message,"withdraw ");
    strcat(message, amount.c_str());

    int n = write(socketNo, message, 16);
    if (n < 0) error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(socketNo,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    std:: cout << buffer << std::endl;
  }

}

void transfer(const std::string sessionKey, std::string amount, std::string username, int socketNo){

  // Check balance for sufficient funds
  int n = write(socketNo, "balance", 16);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  std:: cout << "balance is " << buffer << std::endl;

  float balance = atof(buffer);
  float amountf = atof(amount.c_str());

  // If sufficient funds, allow transfer
  if(amountf < balance){

    // Check if other user exitss
    bzero(buffer,256);
    strcpy (buffer,"withdraw ");
    strcat(buffer, amount.c_str());
    int n = write(socketNo, buffer, 16);
    if (n < 0) error("ERROR writing to socket");


    bzero(buffer,256);
    n = read(socketNo,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    std:: cout << "Response is  " << buffer << std::endl;

    if(true){

      char message[256];
      strcpy (message,"transfer ");
      strcat(message, amount.c_str());
      strcat(message, " ");
      strcat(message, username.c_str());

      int n = write(socketNo, message, 16);
      if (n < 0) error("ERROR writing to socket");

      bzero(buffer,256);
      n = read(socketNo,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      std:: cout << buffer << std::endl;
    }
  }

}
