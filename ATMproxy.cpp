

#include "PracticalSocket.h"
#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()
#include <sys/ioctl.h>        // For  ioctl()

using namespace std;

const unsigned int RCVBUFSIZE = 32;    // Size of receive buffer

void HandleTCPClient(TCPSocket *sock); // TCP client handling function

int main(int argc, char *argv[]) {
  if ((argc < 2) || (argc > 2)) {
    cerr << "Usage: " << argv[0] << " <Listen Port> <Server Port>" << endl;
    exit(1);
  }

  unsigned short listen_port = argv[1]; // listening port, from ATMclient to ATMproxy
  unsigned short serv_port = argv[2];   // server port, from ATMproxy to ATMclient
  string serv_address = "127.0.0.1";
  char* data;

  /*try {
    TCPSocket sock(servAddress, echoServPort);
  }*/
  try {
    TCPServerSocket proxy_sock(listen_port);     // Listen to port

    TCPSocket client_sock = proxy_sock.accept();       // Wait for the client to connect
    TCPSocket server_sock(serv_address, serv_port);     // Connect to server

    //If server sends data send it to the client
    char data_buffer[RCVBUFSIZE];
    int data_size;
    while ((data_size = server_sock.recv(data_buffer, RCVBUFSIZE)) > 0) {
      client_sock.send(data_buffer, data_size);
    }

    //If client sends data send it to the server
    while ((data_size = client_sock.recv(data_buffer, RCVBUFSIZE)) > 0) {
      server_sock.send(data_buffer, data_size);
    }

  } catch (SocketException &e) {
    cerr << e.what() << endl;
    exit(1);
  }

  return 0;
}

void HandleTCPClient(TCPSocket *sock) {
  // Send received string and receive again until the end of transmission
  char dataBuffer[RCVBUFSIZE];
  int recvMsgSize;
  while ((recvMsgSize = sock->recv(dataBuffer, RCVBUFSIZE)) > 0) { // Zero means end of transmission
    // Echo message back to client
    sock->send(dataBuffer, recvMsgSize);
  }
  delete sock;
}
