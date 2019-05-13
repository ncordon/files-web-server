#include "file.h"
#include "server.h"
#include <iostream>

using std::cout;
using std::endl;
  
int main(int argc, char* argv[]) {

  if (argc != 3) {
    cout << "Usage:" << argv[0] << " <port for the server> <folder path>" << endl;
    return -1;
  }

  // The port where the server is going to be
  // binded and the root folder we want to index
  int port = std::stoi(argv[1]);
  string root_folder = argv[2];

  FileServer server(port, root_folder);
  
  while(true) {
    Request request = server.await_request();
    request.answer();
  }
} 
