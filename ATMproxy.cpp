/*
  Proxy.cpp

*/


#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void* client_thread(void* arg);
int bankSocket; //Global FDs for network sockets
int atmSocket;
unsigned short bankPortNo;

struct thread_info {    		/* Used as argument to thread_start() */
	pthread_t thread_id;     /* ID returned by pthread_create() */
	int arg;
};

void closeSocket(int param) {
	printf("\nGracefully shutting down...\n");
	fflush(stdout);
	close(bankSocket);
	close(atmSocket);
	exit(1);
}

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("Usage: proxy listen-port bank-connect-port\n");
		return -1;
	}

	//Close sock on Ctrl-C
	signal(SIGINT, closeSocket);

  // Parse Arguments for ports
	unsigned short atmPortNo = atoi(argv[1]);
	bankPortNo = atoi(argv[2]);

  // Create a socket(New Connection Endpoint)
  // ARG 0, AF_INET - Adress Domain
  // ARG 1, SOCK_STREAM -  Type of socket (This is a stream)
  // ARG 2, Protocol (zero means the OS will choose the most appropriate)
	atmSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(atmSocket == -1) {
		printf("Socket Not Created\n");
		return -1;
	}

  /*
    sockaddr_in is a structure containing an internet address (defined in netinet/in.h)
    struct sockaddr_in
    {
      short   sin_family;   // must be AF_INET
      u_short sin_port;
      struct  in_addr sin_addr;
      char    sin_zero[8];  // Not used, must be zero
    };
  */
	sockaddr_in addr_l;
	addr_l.sin_family = AF_INET;
	addr_l.sin_port = htons(atmPortNo);
  addr_l.sin_addr.s_addr = INADDR_ANY;

  if (bind(atmSocket, (struct sockaddr *) &addr_l, sizeof(addr_l)) < 0){
         error("ERROR on binding");
  }

  listen(atmSocket,SOMAXCONN);

	//loop forever accepting new connections
	pthread_t thread_id = 0;
	while(1) {
		sockaddr_in unused;
		socklen_t size = sizeof(unused);
		int csock = accept(atmSocket, reinterpret_cast<sockaddr*>(&unused), &size);
		if(csock < 0)	//bad client, skip it
			continue;

		thread_info t;
		t.arg = csock;
		t.thread_id = thread_id++;

		pthread_t thread;
		pthread_create(&thread, NULL, client_thread, &t);
	}
}

/*
  Create a client thread on new socket connection
*/

void* client_thread(void* arg) {
	struct thread_info *tinfo;
	tinfo = (thread_info *) arg;
	int csock = tinfo->arg;

	printf("[proxy] client ID #%d connected\n", csock);

	// New socket to connect to the bank
	bankSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(bankSocket == -1) {
    printf("Socket Not Created\n");
		return NULL;
	}

	sockaddr_in addr_b;
	addr_b.sin_family = AF_INET;
	addr_b.sin_port = htons(bankPortNo);
  addr_b.sin_addr.s_addr = INADDR_ANY;

	if(0 != connect(bankSocket, reinterpret_cast<sockaddr*>(&addr_b), sizeof(addr_b))) {
		printf("fail to connect to bank\n");
		return NULL;
	}

	//input loop
	unsigned int length;
	char packet[1024];
	while(1) {
		//read the packet from the ATM
    // ssize_t recv(int sockfd, void *buf, size_t len, int flags);
    /*
    s [in] The descriptor that identifies a connected socket.
    buf [out] A pointer to the buffer to receive the incoming data.
    len [in] The length, in bytes, of the buffer pointed to by the buf parameter.
    flags [in] A set of flags that influences the behavior of this function.
    */



		if(sizeof(int) != recv(csock, &length, sizeof(int), 0))
			break;
		if(length >= 1024) {
			printf("packet too long\n");
			break;
		}
		if(length != recv(csock, packet, length, 0)) {
			printf("[proxy] fail to read packet\n");
			break;
		}

    /*
      Other Team may mess with code here.
      Good Luck!
    */

		//forward packet to bank
		if(sizeof(int) != send(bankSocket, &length, sizeof(int), 0)) {
			printf("[proxy] fail to send packet length\n");
			break;
		}
		if(length != send(bankSocket, (void*)packet, length, 0)) {
			printf("[proxy] fail to send packet\n");
			break;
		}

		//get response packet from bank
		if(sizeof(int) != recv(bankSocket, &length, sizeof(int), 0)) {
			printf("[proxy] fail to read packet length\n");
			break;
		}
		if(length >= 1024) {
			printf("packet too long\n");
			break;
		}
		if(length != recv(bankSocket, packet, length, 0)) {
			printf("[proxy] fail to read packet\n");
			break;
		}

    /*
      Other Team may mess with code here.
      Good Luck!
    */

		//forward packet to ATM
		if(sizeof(int) != send(csock, &length, sizeof(int), 0)) {
			printf("[proxy] fail to send packet length\n");
			break;
		}
		if(length != send(csock, (void*)packet, length, 0)) {
			printf("[proxy] fail to send packet\n");
			break;
		}

	}

	printf("[proxy] client ID #%d disconnected\n", csock);

	close(bankSocket);
	close(csock);
	return NULL;
}
