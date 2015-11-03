
#include "PracticalSocket.h"  // For Socket, ServerSocket, and SocketException
#include <iostream>

const unsigned int RCVBUFSIZE = 64;    // Size of receive buffer


// use 2 threads
//  one UDPSocket thread
//  one terminal input thread


int main(int argc, char *argv[]) {
  if ((argc < 3) || (argc > 4)) {     // Test for correct number of arguments
    cerr << "Usage: " << argv[0]
         << " <Server> <Echo String> [<Server Port>]" << endl;
    exit(1);
  }

  TCPServerSocket proxy_sock(listen_port);

  return 0;
}
