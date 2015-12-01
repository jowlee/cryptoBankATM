#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include "crypto.h"

#define write cwrite
#define read cread


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

std::string login(const std::string username, int socketNo){

  std::string cardInfo;
  char cardName[80];
  strcpy (cardName,username.c_str());
  strcat(cardName, ".card");

  std::ifstream inFile( cardName);

  std::string str;
  if(! inFile){
    std::cout << "Broken... Couldn't open file" << std::endl;
    return "broken";
  }

  if(!std::getline(inFile, str)){
    std::cout << "Broken..." << std::endl;
    return "broken";
  }

  std::string password;
  std::cout << "Password : ";
  std::cin >> password;

  if(!validPin(password)){
    std::cout << "Error Incorrect Password" << std::endl;
    return "broken";
  }

  char message[256];

  strcpy(message, "login ");
  strcat(message, username.c_str());
  strcat(message, " ");
  strcat(message, password.c_str());

  int n = write(socketNo, message, strlen(message)+1);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");

  std::string response(buffer);
  int index = 0;
  std::string works = advanceCommand(response, index);

  if(works.compare("y") == 0){
    advanceSpaces(response, index);
    std::string code = advanceCommand(response, index);
    return code;
  }
  else{
    std:: cout << "Didn't work .... sorry" << std::endl;
    return "broken";
  }

}

void balance(const std::string sessionKey, int socketNo){

  char message[256];
  strcpy (message, sessionKey.c_str());
  strcat(message, " balance");

  int n = write(socketNo, message, strlen(message)+1);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  if(checkGood(std::string(buffer))){
    return;
  }
  std::cout << "balance is " << buffer << std::endl;
}

void withdraw(const std::string sessionKey, std::string amount, int socketNo){

  // Check balance for sufficient funds
  char message[256];
  strcpy (message, sessionKey.c_str());
  strcat(message, " balance");
  int n = write(socketNo, message, strlen(message)+1);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  if(checkGood(std::string(buffer))){
    return;
  }

  float balance = atof(buffer);
  float amountf = atof(amount.c_str());

  // If sufficient funds, allow withdraw
  if(amountf < balance){
    bzero(message,256);
    strcpy (message, sessionKey.c_str());
    strcat (message," withdraw ");
    strcat(message, amount.c_str());

    int n = write(socketNo, message, strlen(message)+1);
    if (n < 0) error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(socketNo,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    if(checkGood(std::string(buffer))){
      return;
    }
    std:: cout << buffer << std::endl;
  }
  else{
    std::cout << "Insufficient funds to transfer" << buffer << std::endl;
  }

}

void transfer(const std::string sessionKey, std::string amount, std::string username, int socketNo){

  // Check balance for sufficient funds
  // Check balance for sufficient funds
  char message[256];
  strcpy (message, sessionKey.c_str());
  strcat(message, " balance");
  int n = write(socketNo, message, strlen(message)+1);
  if (n < 0) error("ERROR writing to socket");

  char buffer[256];
  bzero(buffer,256);
  n = read(socketNo,buffer,255);
  if (n < 0) error("ERROR reading from socket");
  if(checkGood(std::string(buffer))){
    return;
  }

  float balance = atof(buffer);
  float amountf = atof(amount.c_str());

  // If sufficient funds, allow transfer
  if(amountf < balance){

    // Check if other user exitss
    bzero(buffer,256);
    strcpy (buffer, sessionKey.c_str());
    strcat (buffer," check ");
    strcat(buffer, username.c_str());
    int n = write(socketNo, buffer, strlen(buffer)+1);
    if (n < 0) error("ERROR writing to socket");

    bzero(buffer,256);
    n = read(socketNo,buffer,255);
    if (n < 0) error("ERROR reading from socket");
    if(checkGood(std::string(buffer))){
      return;
    }
    std:: cout << "Response is  " << buffer << std::endl;



    if((std::string(buffer)).compare("y") == 0){
      bzero(message,256);
      strcpy (message, sessionKey.c_str());
      strcat (message," transfer ");
      strcat(message, amount.c_str());
      strcat(message, " ");
      strcat(message, username.c_str());

      int n = write(socketNo, message, strlen(message)+1);
      if (n < 0) error("ERROR writing to socket");

      bzero(buffer,256);
      n = read(socketNo,buffer,255);
      if (n < 0) error("ERROR reading from socket");
      if(checkGood(std::string(buffer))){
        return;
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

}
