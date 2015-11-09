#include "crypto.h"

const int PACKET_DATA_LENGTH = 32;    //bytes
const int PACKET_CHECKSUM_LENGTH = 32;

// send command
ssize_t send(TCPSocket *sock, const void *buf, size_t len, int flags){
	//long indexOfPad = random integer from 0 to size of OTP in bytes
	//byte* nonce = longToByteArray(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
	//send nonce to reciever
	//1
	//long myChecksum = xor(OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH), OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH));
	//2
	//byte* response = response from receiver
	//if myChecksum != response {
		//error
		//retry, abort, ignore?
	//}
	//send ok to reciever
	//2.1
	//2.2
	//for(int bufferIndex = 0; bufferIndex < len; bufferIndex += PACKET_DATA_LENGTH) {
		//byte* toSend = buf[bufferIndex];
		//byte* key = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		//byte* mac = sha-256(concat(toSend, key));
		//byte* message = concat(toSend, mac);
		//byte* pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		//send xor(message, pad) to reciever 
		//3
	//}
}

ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);

// recv command
ssize_t recv(TCPSocket *sock, void *buf, size_t len, int flags) {
	//1
	//wait for nonce from sender
	//long indexOfPad;
	//indexOfPad = byteArrayToLong(message recieved from sender);
	//byte* message = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
	//byte* pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
	//byte* reply = xor(message, pad);
	//send reply to sender
	//2
	//2.1
	//recieve ok, if not retry, abort, ignore?
	//2.2
	//int amount_recv = 0;
	//while(more to recieve or amount_recv + PACKET_DATA_LENGTH > len) {
		//3
		//byte data[PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH] = message recieved from sender
		//byte* key = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		//pad = OTP(&indexOfPad, PACKET_DATA_LENGTH + PACKET_CHECKSUM_LENGTH);
		//byte* messsage = xor(data, pad);
		//byte* checksum = data + PACKET_DATA_LENGTH;     checksum is last part after the data
		//byte* mac = sha-256(concat(messsage, key));     recompute mac
		//if(checksum != mac) {
			//error
			//retry, abort, ignore?
		//}
		//for(int i = 0; i < PACKET_DATA_LENGTH; i++) {
			//buf[i] = messsage[i];
		//}
		//buf += PACKET_DATA_LENGTH;
		//amount_recv += PACKET_DATA_LENGTH;
	//}
	//return amount_recv;	
}
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);

byte* OTP(long* index, long amount) {
	//byte pad[amount];  allocate amount bytes to return
	//seek to index in OTP
	//for(long i = 0; i < amount; i++) {
		//pad[i] = readByte();  read in each byte from the file
	//}
	//*index += amount;  increment index so we don't use the same pad twice
	//return pad;
}

byte* xor(byte* first, byte* second, long length) {
	//byte result[length];
	//for(long i = 0; i < length; i++) {
		//result[i] = first[i] ^ second[i];
	//}
	//return result;
}
