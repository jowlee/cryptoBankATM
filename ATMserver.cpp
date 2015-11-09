
#include "PracticalSocket.h"	// For Socket, ServerSocket, and SocketException
#include <iostream>				// For cerr and cout
#include <string>				// For Strings
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

void *socketThread(void *threadid){
	
	pthread_exit(NULL);		// Ends the thread
}