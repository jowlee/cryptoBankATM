

#include <iostream>           // For cerr and cout
#include <cstdlib>            // For atoi()

using namespace std;

int main(int argc, char *argv[]) {
  if ((argc < 2) || (argc > 2)) {
    cerr << "Usage: " << argv[0] << " <Listen Port> <Server Port>" << endl;
    exit(1);
  }
  
  return 0;
}

