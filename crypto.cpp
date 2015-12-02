#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <math.h>
#include "sha256.h"
#include "crypto.h"

const int PACKET_DATA_LENGTH = 32;    //bytes
const int PACKET_CHECKSUM_LENGTH = 32;
const int PACKET_LENGTH = PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH;

ssize_t swrite(int fd, const void *buf, size_t count);
ssize_t sread(int fd, void *buf, size_t count);
char* OTP(unsigned long long* index, const unsigned long long amount);
char* xorCharArray(const char* first, const char* second, unsigned long long length);
unsigned long long unsignedLongLongRand();
char* longToCharArray(unsigned long long num, int size);
unsigned long long charArrayToLong(const char* data, int size);
bool charArrayEquals(const char* data1, const char* data2, int size);
char* concat(const char* left, const char* right, int sizel, int sizer);
char* sha_256(char* buf, int length);


// send command
ssize_t cwrite(int fd, const void *buf, size_t len) {
	srand (time(NULL));
	#ifdef _DEBUG
	std::cout << "Sending a message" << std::endl;
	#endif
	//long indexOfPad = random integer from 0 to size of OTP in bytes
	unsigned long long* indexOfPad = new unsigned long long;
	*indexOfPad = unsignedLongLongRand();
	//char* nonce = longToCharArray(&indexOfPad, PACKET_LENGTH);
	char* nonce = longToCharArray(*indexOfPad, PACKET_LENGTH);
	//send nonce to reciever
	int n;
	//n = swrite(fd, nonce, PACKET_LENGTH);
	n = write(fd, nonce, PACKET_LENGTH);
	delete nonce;
	if (n < 0) {
		return -1;
	}
	//1
	//char* myChecksum = xor(OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH), OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH));
	char* p1 = OTP(indexOfPad,PACKET_LENGTH);
	char* p2 = OTP(indexOfPad,PACKET_LENGTH);
	char* myChecksum = xorCharArray(p1, p2, PACKET_LENGTH);
	//2
	//byte* response = response from receiver
	char buffer[PACKET_LENGTH];
	n = read(fd, buffer, PACKET_LENGTH);
	if (n < 0) {
		//error
		n = write(fd, "NO", 2);
		if (n < 0) {
			return -1;
		}
		return -1;
	}
	else if (n == 0) {
		//error - no message
		n = write(fd, "NO", 2);
		if (n < 0) {
			return -1;
		}
		return -1;
	}
	else if(!charArrayEquals(buffer, myChecksum, PACKET_LENGTH)) {
		//myChecksum != response
		return -1;
	}
	//send ok to reciever
	n = write(fd, "OK", 2);
	if (n < 0) {
		return -1;
	}
	//2.1
	//2.2
	//for(int bufferIndex = 0; bufferIndex < len; bufferIndex += PACKET_DATA_LENGTH) {
	for(int bufferIndex = 0; bufferIndex < len; bufferIndex += PACKET_DATA_LENGTH) {
		//char* toSend = buf[bufferIndex];
		
		char* toSend = new char[PACKET_DATA_LENGTH];
		bzero(toSend, PACKET_DATA_LENGTH);
		for(int i = 0; i < PACKET_DATA_LENGTH && bufferIndex + i < len; i++) {
			toSend[i] = ((char*)buf)[bufferIndex + i];
		}
		
		#ifdef _DEBUG
		std::cout << "toSend: ";
		for(int i = 0; i < PACKET_DATA_LENGTH; i++) std::cout << toSend[i];
		std::cout << std::endl;
		#endif
		
		//char* key = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		char* key = OTP(indexOfPad, PACKET_LENGTH);
		#ifdef _DEBUG
		std::cout << "key: ";
		for(int i = 0; i < PACKET_LENGTH; i++) std::cout << key[i];
		std::cout << std::endl;
		#endif
		
		//char* mac = sha-256(concat(toSend, key));
		char* mac = sha_256(concat(toSend, key, PACKET_DATA_LENGTH, PACKET_CHECKSUM_LENGTH), PACKET_LENGTH);
		delete[] key;
		#ifdef _DEBUG
		std::cout << "mac: ";
		for(int i = 0; i < PACKET_CHECKSUM_LENGTH; i++) std::cout << mac[i];
		std::cout << std::endl;
		#endif
		
		//char* message = concat(toSend, mac);
		char* message = concat(toSend, mac, PACKET_DATA_LENGTH, PACKET_CHECKSUM_LENGTH);
		
		delete[] mac;
		#ifdef _DEBUG
		std::cout << "message: ";
		for(int i = 0; i < PACKET_LENGTH; i++) std::cout << message[i];
		std::cout << std::endl;
		#endif
		
		//char* pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		char* pad = OTP(indexOfPad, PACKET_LENGTH);
		
		#ifdef _DEBUG
		std::cout << "pad: ";
		for(int i = 0; i < PACKET_LENGTH; i++) std::cout << pad[i];
		std::cout << std::endl;
		#endif
		
		//send xor(message, pad) to reciever 
		#ifdef _DEBUG
		std::cout << "Sending encrypted message" << std::endl;
		#endif
		char* enc = xorCharArray(message, pad, PACKET_LENGTH);
		delete[] message;
		delete[] pad;
		n = write(fd, enc, PACKET_LENGTH);
		delete[] enc;
		if (n < 0) {
			return -1;
		}
		#ifdef _DEBUG
		std::cout << "Sent encrypted message" << std::endl;
		#endif
		n = read(fd, buffer, 2);
		buffer[2] = '\0';
		#ifdef _DEBUG
		std::cout << "Got confirmation: " << buffer << std::endl;
		#endif
		if (n < 0) {
			//error
			return -1;
		}
		else if (n == 0) {
			//error - no message
			return -1;
		}
		else if(buffer[0] != 'O' || buffer[1] != 'K') {
			//error - not ok
			#ifdef _DEBUG
			std::cout << "Not OK: " << (buffer[0] == 'O') << " " << (buffer[1] == 'K') << std::endl;
			#endif
			return -1;
		}
		//3
	}
	#ifdef _DEBUG
	std::cout << "Done sending encrypted message" << std::endl;
	#endif
	n = write(fd, "DONE", 4);
	return len;
}

// recv command
ssize_t cread(int fd, void *buf, size_t len) {
	#ifdef _DEBUG
	std::cout << "Recieving a message" << std::endl;
	#endif
	//1
	//wait for nonce from sender
	char buffer[PACKET_LENGTH];
	int n;
	//n = sread(fd, buffer, PACKET_LENGTH);
	n = read(fd, buffer, PACKET_LENGTH);
	if (n < 0) {
		//error
		return -1;
	}
	else if (n == 0) {
		//error - no message
		return -1;
	}
	
	#ifdef _DEBUG
	std::cout << "buffer: ";
	for(int i = 0; i < PACKET_LENGTH; i++) std::cout << (int)buffer[i];
	std::cout << std::endl;
	#endif
	
	//long indexOfPad;
	//indexOfPad = charArrayToLong(message recieved from sender);
	unsigned long long indexOfPad = charArrayToLong(buffer, PACKET_LENGTH);
	#ifdef _DEBUG
	std::cout << "indexOfPad: " << indexOfPad << std::endl;
	#endif
	
	char* message = OTP(&indexOfPad, PACKET_LENGTH);
	
	#ifdef _DEBUG
	std::cout << "message: ";
	for(int i = 0; i < PACKET_LENGTH; i++) std::cout << message[i];
	std::cout << std::endl;
	#endif
	
	//byte* pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
	char* pad = OTP(&indexOfPad, PACKET_LENGTH);
	
	#ifdef _DEBUG
	std::cout << "pad: ";
	for(int i = 0; i < PACKET_LENGTH; i++) std::cout << pad[i];
	std::cout << std::endl;
	#endif
	
	//byte* reply = xor(message, pad);
	char* reply = xorCharArray(message, pad, PACKET_LENGTH);
	
	#ifdef _DEBUG
	std::cout << "reply: ";
	for(int i = 0; i < PACKET_LENGTH; i++) std::cout << reply[i];
	std::cout << std::endl;
	#endif
	
	#ifdef _DEBUG
	std::cout << "Sending reply" << std::endl;
	#endif
	//send reply to sender
	n = write(fd, reply, PACKET_LENGTH);
	#ifdef _DEBUG
	std::cout << "Sent reply" << std::endl;
	#endif
	if (n < 0) {
		return -1;
	}
	//2
	//2.1
	//recieve ok, if not retry, abort, ignore?
	#ifdef _DEBUG
	std::cout << "Waiting for confirmation" << std::endl;
	#endif
	n = read(fd, buffer, 2);
	buffer[2] = '\0';
	#ifdef _DEBUG
	std::cout << "Got confirmation: " << buffer << std::endl;
	#endif
	if (n < 0) {
		//error
		return -1;
	}
	else if (n == 0) {
		//error - no message
		return -1;
	}
	else if(buffer[0] != 'O' || buffer[1] != 'K') {
		//error - not ok
		#ifdef _DEBUG
		std::cout << "Not OK: " << (buffer[0] == 'O') << " " << (buffer[1] == 'K') << std::endl;
		#endif
		return -1;
	}
	//2.2
	//int amount_recv = 0;
	int amount_recv = 0;
	//while(more to recieve or amount_recv + PACKET_DATA_LENGTH < len) {
	#ifdef _DEBUG
	std::cout << "Recieving data" << std::endl;
	#endif
	while(amount_recv + PACKET_DATA_LENGTH < len) {
		//3
		//byte data[PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH] = message recieved from sender
		char data[PACKET_LENGTH];
		bzero(data, PACKET_LENGTH);
		#ifdef _DEBUG
		std::cout << "Waiting for data" << std::endl;
		#endif
		n = read(fd, data, PACKET_LENGTH);
		if (n < 0) {
			//error
			return -1;
		}
		else if (n == 0) {
			//done - no message
			return amount_recv;
		}
		else if (data[0] == 'D' && data[1] == 'O' && data[2] == 'N' && data[3] == 'E') {
			return amount_recv;
		}
		amount_recv += n;
		#ifdef _DEBUG
		std::cout << "Got data" << std::endl;
		#endif
		//byte* key = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		char* key = OTP(&indexOfPad, PACKET_LENGTH);
		
		#ifdef _DEBUG
		std::cout << "key: ";
		for(int i = 0; i < PACKET_LENGTH; i++) std::cout << key[i];
		std::cout << std::endl;
		#endif
	
		//pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		char* pad = OTP(&indexOfPad, PACKET_LENGTH);
		
		#ifdef _DEBUG
		std::cout << "pad: ";
		for(int i = 0; i < PACKET_LENGTH; i++) std::cout << pad[i];
		std::cout << std::endl;
		#endif
		
		//byte* messsage = xor(data, pad);
		char* message = xorCharArray(data, pad, PACKET_LENGTH);
		
		#ifdef _DEBUG
		std::cout << "message: ";
		for(int i = 0; i < PACKET_LENGTH; i++) std::cout << message[i];
		std::cout << std::endl;
		#endif
		
		//byte* checksum = data + PACKET_DATA_LENGTH;     checksum is last part after the data
		char* checksum = message + PACKET_DATA_LENGTH;
		
		#ifdef _DEBUG
		std::cout << "checksum: ";
		for(int i = 0; i < PACKET_CHECKSUM_LENGTH; i++) std::cout << checksum[i];
		std::cout << std::endl;
		#endif

		char* mac = sha_256(concat(message, key, PACKET_DATA_LENGTH, PACKET_CHECKSUM_LENGTH), PACKET_LENGTH);
		
		#ifdef _DEBUG
		std::cout << "mac: ";
		for(int i = 0; i < PACKET_CHECKSUM_LENGTH; i++) std::cout << mac[i];
		std::cout << std::endl;
		#endif
		
		if(!charArrayEquals(checksum, mac, PACKET_CHECKSUM_LENGTH)) {
			//checksum != mac
			n = write(fd, "NO", 2);
			#ifdef _DEBUG
			std::cout << "Invalid checksum" << std::endl;
			#endif
			delete[] mac;
			return -1;
		}
		delete[] mac;
		n = write(fd, "OK", 2);
		if (n < 0) {
			return -1;
		}
		
		//copy message into buffer
		#ifdef _DEBUG
		std::cout << "message: " << std::flush;
		#endif
		for(int i = 0; i < PACKET_DATA_LENGTH; i++) {
			((char*)buf)[i] = message[i];
			#ifdef _DEBUG
			std::cout << message[i] << std::flush;
			#endif
		}
		#ifdef _DEBUG
		std::cout << std::endl;
		#endif
		//memcpy(buf, message, PACKET_DATA_LENGTH);
		//for(int i = 0; i < PACKET_DATA_LENGTH; i++) {
		//	buf[i] = message[i];
		//}
		buf += PACKET_DATA_LENGTH;
		
	}
	return amount_recv;	
}

ssize_t swrite(int fd, const void *buf, size_t count) {
	int amount = 50;
	int amount_good = 20;
	char* salt = "SALT";
	int saltLength = 4;
	char* data = new char[count];
	int blocksize = count + PACKET_CHECKSUM_LENGTH;
	for(int i = 0; i < count; i++) {
		data[i] = ((char*)buf)[i];
	}
	char** block = new char*[amount];
	
	std::ifstream urandom("/dev/urandom", std::ifstream::in|std::ifstream::binary);
	for(int i = 0; i < amount; i++) {
		block[i] = new char[blocksize];
		urandom.read(block[i], count);
	}
	
	for(int i = 0; i < amount_good-1; i++) {
		char* newData = xorCharArray(data, block[i], count);
		//std::cout << "deleting data" << std::endl;
		delete[] data;
		data = newData;
	}
	block[amount_good-1] = data;
	for(int i = 0; i < amount_good; i++) {
		char* salted = concat(block[i], salt, count, saltLength);
		char tmpsalted[count + saltLength + 1];
		for(int j = 0; j < count + saltLength; j++) tmpsalted[j] = salted[j];
		tmpsalted[count + saltLength] = '\0';
		char* MAC = sha_256(tmpsalted, count + saltLength + 1);
		char* tmpblock = concat(block[i], MAC, count, PACKET_CHECKSUM_LENGTH);
		
		#ifdef _DEBUG
		std::cout << "salted: ";
		for(int j = 0; j < count + saltLength; j++) std::cout << (unsigned int)(unsigned char)tmpsalted[j];
		std::cout << std::endl;
		#endif
		
		#ifdef _DEBUG
		std::cout << "MAC: ";
		for(int j = 0; j < PACKET_CHECKSUM_LENGTH; j++) std::cout << (unsigned int)(unsigned char)MAC[j];
		std::cout << std::endl;
		#endif
		
		#ifdef _DEBUG
		std::cout << "making block: ";
		for(int j = 0; j < blocksize; j++) std::cout << (unsigned int)(unsigned char)tmpblock[j];
		std::cout << std::endl;
		#endif
		
		delete[] MAC;
		//std::cout << "deleting block[i]" << std::endl;
		delete[] block[i];
		//std::cout << "deleting salted" << std::endl;
		delete salted;
		block[i] = tmpblock;
	}
	for(int i = amount_good; i < amount; i++) {
		char fakeMAC[PACKET_CHECKSUM_LENGTH];
		urandom.read(fakeMAC, PACKET_CHECKSUM_LENGTH);
		char* tmpblock = concat(block[i], fakeMAC, count, PACKET_CHECKSUM_LENGTH);
		//std::cout << "deleting block[i] (2)" << std::endl;
		delete[] block[i];
		block[i] = tmpblock;
	}
	for(int i = 0; i < 1000; i++) {
		int first = i % amount;
		int second = rand() % amount;
		char* tmp = block[first];
		block[first] = block[second];
		block[second] = tmp;
	}
	urandom.close();
	//n = write(fd, (size_t)(count + PACKET_CHECKSUM_LENGTH), sizeof(size_t));
	for(int i = 0; i < amount; i++) {
		
		//std::cout << "sending block: ";
		//for(int j = 0; j < blocksize; j++) std::cout << block[i][j];
		//std::cout << std::endl;
		
		int n = write(fd, block[i], blocksize);
		//std::cout << "deleting block[i] (3)" << std::endl;
		delete[] block[i];
		if (n < 0) {
			return -1;
		}
	}
	//std::cout << "deleting block" << std::endl;
	delete[] block;
	return count;
	//int n = write(fd, send.c_str(), send.length());
    
}
ssize_t sread(int fd, void *buf, size_t count) {
	int amount = 50;
	int amount_good = 20;
	char* salt = "SALT";
	int saltLength = 4;
	char* data = new char[count];
	for(int i = 0; i < count; i++) {
		data[i] = 0;
	}
	//char** block = new char[amount][count + PACKET_CHECKSUM_LENGTH];
	static int blocksize = count + PACKET_CHECKSUM_LENGTH;
	char** block = new char*[amount_good];
	int block_index = 0;
	for(int i = 0; i < amount; i++) {
		char* tmpblock = new char[blocksize];
		int n = read(fd, tmpblock, blocksize);
		if (n < 0) {
			return -1;
		}
		char* checksum = tmpblock + count;
		
		char* salted = concat(tmpblock, salt, count, saltLength);
		char tmpsalted[count + saltLength + 1];
		for(int j = 0; j < count + saltLength; j++) tmpsalted[j] = salted[j];
		tmpsalted[count + saltLength] = '\0';
		char* MAC = sha_256(tmpsalted, count + saltLength + 1);
		#ifdef _DEBUG
		std::cout << "making salted: ";
		for(int j = 0; j < count + saltLength; j++) std::cout << (unsigned int)(unsigned char)tmpsalted[j];
		std::cout << std::endl;
		#endif
		#ifdef _DEBUG
		std::cout << "MAC     : ";
		for(int j = 0; j < PACKET_CHECKSUM_LENGTH; j++) std::cout << (unsigned int)(unsigned char)MAC[j];
		std::cout << std::endl;
		//std::cout << "checksum: " << checksum << std::endl;
		//std::cout << "MAC     : " << (char*)MAC << std::endl;
		#endif
		#ifdef _DEBUG
		std::cout << "checksum: ";
		for(int j = 0; j < PACKET_CHECKSUM_LENGTH; j++) std::cout << (unsigned int)(unsigned char)checksum[j];
		std::cout << std::endl;
		#endif
		
		//std::cout << "Delete MAC"<< std::endl;
		
		delete[] salted;
		if(charArrayEquals(checksum, MAC, PACKET_CHECKSUM_LENGTH)) {
			//std::cout << "Good Block: " << i << std::endl;
			block[block_index++] = tmpblock;
		}
		//else {
		//	std::cout << "Bad Block: " << i << std::endl;
		//}
		delete[] MAC;
	}
	for(int i = 0; i < amount_good; i++) {
		char* tmp = xorCharArray(data, block[i], count);
		//std::cout << "deleting block[i]" << std::endl;
		delete[] block[i];
		//std::cout << "deleting data" << std::endl;
		delete[] data;
		data = tmp;
	}
	//std::cout << "deleting block" << std::endl;
	delete[] block;
	for(int i = 0; i < count; i++) {
		((char*)buf)[i] = data[i];
	}
	//std::cout << "deleting data" << std::endl;
	delete[] data;
	return count;
}

char* OTP(unsigned long long* index, const unsigned long long amount) {
	std::ifstream pad("otp.key", std::ifstream::ate | std::ifstream::binary);
	//std::ifstream pad;
	//pad.open("otp.key");
	pad.seekg(0,std::ios::end);
	pad.seekg(*index % pad.tellg());
	char* result = new char[amount];
	pad.read(result, amount);
	pad.close();
	*index += amount;
	return result;
	//byte pad[amount];  allocate amount bytes to return
	//seek to index in OTP
	//for(long i = 0; i < amount; i++)
		//pad[i] = readByte();  read in each byte from the file
	//*index += amount;  increment index so we don't use the same pad twice
	//return pad;
}

char* xorCharArray(const char* first, const char* second, unsigned long long length) {
	//byte result[length];
	char* result = new char[length];
	//for(long i = 0; i < length; i++) {
	for(unsigned long long i = 0; i < length; i++) {
		result[i] = first[i] ^ second[i];
	}
	return result;
}

unsigned long long unsignedLongLongRand() {
    unsigned long long rand1 = abs(rand());
    unsigned long long rand2 = abs(rand());
    rand1 <<= (sizeof(int)*8);   
    unsigned long long randULL = (rand1 | rand2);   
    return randULL;
}

char* longToCharArray(unsigned long long num, int size) {
	char* result = new char[size];
	for(int i = 0; i < sizeof(unsigned long long); i++){
		result[i] = num % (int)pow (2,(sizeof(char) * 8));
		num /= (int)pow(2,(sizeof(char) * 8));
	}
	return result;
}

unsigned long long charArrayToLong(const char* data, int size){
	unsigned long long result = 0;
	for(int i = 0; i < size; i++){
		result += ((unsigned long long)(unsigned char)data[i]) * (unsigned long long)pow(2,i*(sizeof(char) * 8));
	}
	return result;
}

bool charArrayEquals(const char* data1, const char* data2, int size) {
	for(int i = 0; i < size; i++) {
		if(data1[i] != data2[i]) return false;
		
	}
	return true;
}

char* concat(const char* left, const char* right, int sizel, int sizer) {
	char* result = new char[sizel + sizer];
	for(int i = 0; i < sizel; i++) {
		result[i] = left[i];
	}
	for(int i = 0; i < sizer; i++) {
		result[sizel + i] = right[i];
	}
	return result;
}

char* sha_256(char* buf, int length) {
	return sha256(buf, length);
}

//For testing     
/*
int main(int argc, char *argv[]){
	srand (time(NULL));
	unsigned long long* indexOfPad = new unsigned long long;
	std::cout << "Index of pad = " << *indexOfPad << std::endl;
	*indexOfPad = unsignedLongLongRand();
	std::cout << "Index of pad = " << *indexOfPad << std::endl;
	char* nonce = longToCharArray(*indexOfPad, PACKET_LENGTH);
	std::cout << "nonce = " << charArrayToLong(nonce, PACKET_LENGTH) << std::endl;
	char* pad1 = OTP(indexOfPad,10);
	char* pad2 = OTP(indexOfPad,10);
	char* padx = xorCharArray(pad1, pad2, 10);
	for(int i = 0; i < 10; i++) {
		std::cout << (unsigned int)(unsigned char)pad1[i] << " ";
	}
	std::cout << std::endl;
	for(int i = 0; i < 10; i++) {
		std::cout << (unsigned int)(unsigned char)pad2[i] << " ";
	}
	std::cout << std::endl;
	for(int i = 0; i < 10; i++) {
		std::cout << (unsigned int)(unsigned char)padx[i] << " ";
	}
	std::cout << std::endl;
	std::cout << charArrayEquals(padx, pad2, 10) << std::endl;
	char* padc = concat(pad1, pad2, 10, 10);
	for(int i = 0; i < 20; i++) {
		std::cout << (unsigned int)(unsigned char)padc[i] << " ";
	}
	std::cout << std::endl;
	std::cout << sha_256(padc) << std::endl;
	std::cout << sha_256(pad1) << std::endl;
	std::cout << sha_256(padc) << std::endl;
	delete nonce;
	delete indexOfPad;
}
//*/