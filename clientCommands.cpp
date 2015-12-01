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
  for(; line[index] != '\n' && line[index] != ' ' && index < line.length(); index++) {
    command += line[index];
  }
  return command;
}
// Skip any number of spaces
void advanceSpaces(const std::string &line, int &index) {
  for(; line[index] == ' ' && index <= line.length(); index++);
}

  // std::string message = "login " + username + " " + password;
  // std::cout << password.c_str() << std::endl;
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
  std:: cout << buffer << std::endl;

  // Do checks
  // Ask for pin and check length is 6

  return "workds";
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
