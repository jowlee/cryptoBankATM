#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string>
#include <cstring>

const int PACKET_DATA_LENGTH = 32;    //bytes
const int PACKET_CHECKSUM_LENGTH = 32;
const int PACKET_LENGTH = PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH;

// send command
ssize_t cwrite(int fd, const void *buf, size_t count) {
	//long indexOfPad = random integer from 0 to size of OTP in bytes
	unsigned long long* indexOfPad = unsignedLongLongRand();
	//char* nonce = longToCharArray(&indexOfPad, PACKET_LENGTH);
	char* nonce = longToCharArray(&indexOfPad, PACKET_LENGTH);
	//send nonce to reciever
	int n;
	n = write(fd, nonce, PACKET_LENGTH);
	if (n < 0) {
		return -1;
	}
	//1
	//long myChecksum = xor(OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH), OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH));
	unsigned long long myChecksum = xorCharArray(OTP(&indexOfPad,PACKET_LENGTH), OTP(&indexOfPad, PACKET_LENGTH));
	//2
	//byte* response = response from receiver
	char buffer[PACKET_LENGTH];
	n = read(fd, buffer, PACKET_LENGTH);
	if (n < 0) {
		//error
		n = write(fd, "NO", PACKET_LENGTH);
		if (n < 0) {
			return -1;
		}
		return -1;
	}
	else if (n == 0) {
		//error - no message
		n = write(fd, "NO", PACKET_LENGTH);
		if (n < 0) {
			return -1;
		}
		return -1;
	}
	else if(!charArrayEquals(buffer, myChecksum)) {
		return -1;
	}
	//if myChecksum != response {
		//error
		//retry, abort, ignore?
		//return -1;
	//}
	//send ok to reciever
	n = write(fd, "OK", PACKET_LENGTH);
	if (n < 0) {
		return -1;
	}
	//2.1
	//2.2
	//for(int bufferIndex = 0; bufferIndex < len; bufferIndex += PACKET_DATA_LENGTH) {
		//char* toSend = buf[bufferIndex];
		//char* key = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		//char* mac = sha-256(concat(toSend, key));
		//char* message = concat(toSend, mac);
		//char* pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		//send xor(message, pad) to reciever 
		//3
	//}
}

// recv command
ssize_t cread(int fd, void *buf, size_t count) {
	//1
	//wait for nonce from sender
	char buffer[PACKET_LENGTH];
	int n;
	n = read(fd, buffer, PACKET_LENGTH);
	if (n < 0) {
		//error
		return -1;
	}
	else if (n == 0) {
		//error - no message
		return -1;
	}
	//long indexOfPad;
	//indexOfPad = charArrayToLong(message recieved from sender);
	unsigned long long indexOfPad = charArrayToLong(buffer);
	//byte* message = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
	char* message = OTP(&indexOfPad, PACKET_LENGTH);
	//byte* pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
	char pad = OTP(&indexOfPad, PACKET_LENGTH);
	//byte* reply = xor(message, pad);
	char* reply = xorCharArray(message, pad);
	//send reply to sender
	n = write(fd, reply, PACKET_LENGTH);
	if (n < 0) {
		return -1;
	}
	//2
	//2.1
	//recieve ok, if not retry, abort, ignore?
	n = read(fd, buffer, PACKET_LENGTH);
	if (n < 0) {
		//error
		return -1;
	}
	else if (n == 0) {
		//error - no message
		return -1;
	}
	else if(std::strcmp(buffer, "OK") != 0) {
		return -1;
	}
	//2.2
	//int amount_recv = 0;
	int amount_recv = 0;
	//while(more to recieve or amount_recv + PACKET_DATA_LENGTH < len) {
	while(amount_recv + PACKET_DATA_LENGTH < len) {
		//3
		//byte data[PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH] = message recieved from sender
		char data[PACKET_LENGTH];
		amount_recv += read(fd, data, PACKET_LENGTH);
		//byte* key = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		char* key = OTP(&indexOfPad, PACKET_LENGTH);
		//pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		char* pad = OTP(&indexOfPad, PACKET_LENGTH);
		//byte* messsage = xor(data, pad);
		char* message = xorCharArray(data, pad, PACKET_LENGTH);
		//byte* checksum = data + PACKET_DATA_LENGTH;     checksum is last part after the data
		char* checksum = message + PACKET_DATA_LENGTH;
		//byte* mac = sha-256(concat(messsage, key));     recompute mac
		std::string msgStr(message);
		std::string keyStr(key);
		char* mac = sha_256(msgStr + keyStr);
		//if(checksum != mac) {
			//error
			//retry, abort, ignore?
			//return -1;
		//}
		//for(int i = 0; i < PACKET_DATA_LENGTH; i++) {
			//buf[i] = messsage[i];
		//}
		//buf += PACKET_DATA_LENGTH;
	}
	return amount_recv;	
}

char* OTP(unsigned long long* index, unsigned long long amount) {
	//byte pad[amount];  allocate amount bytes to return
	//seek to index in OTP
	//for(long i = 0; i < amount; i++) {
		//pad[i] = readByte();  read in each byte from the file
	//}
	//*index += amount;  increment index so we don't use the same pad twice
	//return pad;
}

char* xorCharArray(const char* first, const char* second, unsigned long long length) {
	//byte result[length];
	//for(long i = 0; i < length; i++) {
		//result[i] = first[i] ^ second[i];
	//}
	//return result;
}

unsigned long long unsignedLongLongRand() {
    unsigned long long rand1 = abs(rand());
    unsigned long long rand2 = abs(rand());
    rand1 <<= (sizeof(int)*8);   
    unsigned long long randULL = (rand1 | rand2);   
    return randULL;
}

char* longToCharArray(unsigned long long num, int size) {
	
}

unsigned long long charArrayToLong(const char* data, int size){
	
}

bool charArrayEquals(const char* data1, const char* data2, int size) {
	
}
