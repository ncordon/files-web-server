#ifndef SERVER__H
#define SERVER__H

#include <unistd.h> 
#include <cstdlib> 
#include <string>
#include <vector>
#include <fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>
#include "file.h"
#define BACKLOG 5

using std::string;
using std::vector;
using std::ifstream;
using std::cout;
using std::endl;

struct HttpResponses {
  string NOT_FOUND();

  string OK(string msg);
};


class Request {
private:
  struct FileRequest {
    string http_method;
    string path;
  };
  
  int fd;
  string root_folder;
  const int BUFFER_SIZE = 1024;
  HttpResponses responses;
  FileRequest file_request;

  void parse_request();

  void list_folder(File& current);

  void print_content(File& current);

  void return_error();
  
public:
  Request(string root_folder, int fd);
  
  void answer();
};

class FileServer {
private: 
  int socket_fd;
  string root_folder;
  
public:
  FileServer(int port, string root_folder);
  
  Request await_request();
};

#endif
