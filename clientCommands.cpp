#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>



void error(const char *msg){
    perror(msg);
    exit(0);
}


// Read input until we read a space, then for each character add it to the command string
std::string advanceCommand(const std::string& line, int &index) {
  std::string command = "";
  for(; line[index] != '\n' && line[index] != ' ' && index < line.length(); index++) {
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
  std::cout << "Card Info" << str << std::endl;

  // Do checks
  // Ask for pin and check length is 6

  return "workds";
}

// Validate that pin is 6 characters long
bool validPin(char pin[]){
  if(strlen(pin) == 6){
    return true;
  }
  return false;
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
