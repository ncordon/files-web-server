#include <unistd.h> 
#include <cstdlib> 
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h> 
#define PORT 80
#define BACKLOG 5

using std::cout;
using std::endl;
using std::string;

struct HttpResponses {
  string NOT_FOUND;

  HttpResponses() {
    // Set generic not found
    NOT_FOUND  = "HTTP/1.1 404 Not Found \n";
    NOT_FOUND += "Content-Type: text/html; charset=utf-8 \n";
    NOT_FOUND += "\n";
    NOT_FOUND += "<!DOCTYPE html> \n";
    NOT_FOUND += "<html> \n";
    NOT_FOUND += "  <body> \n";
    NOT_FOUND += "    <h1> Not Found </h1> \n";
    NOT_FOUND += "  </body> \n";
    NOT_FOUND += "</html> \n";
  }

  string OK(string msg) {
    string result;
    result  = "HTTP/1.1 200 OK \n";
    result += "Content-Type: text/html; charset=utf-8 \n";
    result += "\n";
    result += msg;
    return result;
  }
};

class File {
private: 
  string path;
  struct stat stat_result;
  
public:  
  File(string path) {
    this->path = path;
    stat(path.c_str(), &stat_result);
  }

  bool is_file() {
    return S_ISREG(stat_result.st_mode);
  }

  bool is_dir() {
    return S_ISDIR(stat_result.st_mode);
  }
};
  
int main(int argc, char* argv[]) {

  if (argc != 3) {
    cout << "Usage:" << argv[0] << " <port for the server> <folder path>" << endl;
    return -1;
  }

  // The port where the server is going to be
  // binded and the root folder we want to index
  int port = std::stoi(argv[1]);
  string root_folder = argv[2];
  
  // Linux file path is at maximum 4096, so the whole
  // HTTP request header should fit in 6000 bytes
  const int BUFFER_SIZE = 6000;
  int current_socket;
  int num_read;
  int opt = 1;
  char buffer[BUFFER_SIZE];
  HttpResponses responses;
  
  // Create file descriptor for the socket
  int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  struct sockaddr_in address;
  socklen_t address_size = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY; 
  address.sin_port = htons(port); 

  // Make the server listen on port
  setsockopt(socket_fd,
             SOL_SOCKET,
             SO_REUSEADDR | SO_REUSEPORT,
             &opt,
             sizeof(opt));
  bind(socket_fd, (struct sockaddr *)&address, sizeof(address));
  // Make the server listen, with at much BACKLOG requests waiting response
  listen(socket_fd, BACKLOG);

  
  struct sockaddr_in their_addr;
  current_socket = accept(socket_fd,
                          (struct sockaddr*) &their_addr,
                          &address_size);
  num_read = read(current_socket , buffer, BUFFER_SIZE);

  send(current_socket,
       responses.NOT_FOUND.c_str(),
       responses.NOT_FOUND.size(),
       0);
} 
