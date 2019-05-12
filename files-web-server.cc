#include <unistd.h> 
#include <cstdlib> 
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h> 
#define BACKLOG 5

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ifstream;

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
  
public:  
  File(string path) {
    if (!path.empty() && path.back() == '/')
      path.pop_back();
    
    this->path = path;
  }

  bool is_file() {
    struct stat stat_result;
    stat(path.c_str(), &stat_result);

    return S_ISREG(stat_result.st_mode);
  }

  bool is_folder() {
    struct stat stat_result;
    stat(path.c_str(), &stat_result);
    
    return S_ISDIR(stat_result.st_mode);
  }

  vector<string> list_files() {
    vector<string> files;
    DIR *directory = opendir(path.c_str());
    struct dirent *entry = readdir(directory);

    while (entry != NULL) {
      files.push_back(string(entry->d_name));
      entry = readdir(directory);
    }
    
    closedir(directory);
    return files;
  }

  File operator /(string file) {
    string sanitized_path = path;

    if (!path.empty() && path.back() == '/')
      sanitized_path.pop_back();

    if (!file.empty() && file.front() == '/')
      file.erase(file.begin());
    
    return File(sanitized_path + "/" + file);
  }

  string get_path() {
    return path;
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

  while(true) {
    struct sockaddr_in their_addr;
    current_socket = accept(socket_fd,
                            (struct sockaddr*) &their_addr,
                            &address_size);
    num_read = read(current_socket , buffer, BUFFER_SIZE);

    string http_method;
    string msg;
    int i = 0;

    while (i < num_read && buffer[i] != ' ') {
      http_method += buffer[i];
      ++i;
    }
  
    if (http_method == "GET") {
      ++i;
      string path;
    
      while (i < num_read && buffer[i] != ' ') {
        path += buffer[i];
        ++i;
      }

      File current = File(root_folder) / path;
      
      if (current.is_folder()) {
        vector<string> files = current.list_files();
        string listing = "<ul> \n";
        
        for (auto f : files) {
          File child = File(path) / f;
          listing += "<li><a href=\"" + child.get_path() + "\">" + f + "</a></li> \n";
        }

        listing += "</ul> \n";
      
        msg = responses.OK(
            "<!DOCTYPE html> \n<html> \n<body> \n" + listing + "</body> </html>");
        send(current_socket, msg.c_str(), msg.size(), 0);
      } else if (current.is_file()) {
        msg = responses.OK("<!DOCTYPE html> \n<html> \n<body> \n<pre> \n");
        send(current_socket, msg.c_str(), msg.size(), 0);
        char read_buffer[BUFFER_SIZE];
        
        ifstream is(current.get_path());
        
        while (is) {
          is.read(read_buffer, BUFFER_SIZE);
          send(current_socket, read_buffer, is.gcount(), 0);
        }

        string end_msg = "\n</pre> \n</body> \n</html>";
        send(current_socket, end_msg.c_str(), end_msg.size(), 0);
      } else {
        msg = responses.NOT_FOUND;
        send(current_socket, msg.c_str(), msg.size(), 0);
      }
    } else {
      msg = responses.NOT_FOUND;
      send(current_socket, msg.c_str(), msg.size(), 0);
    }

    close(current_socket);
  }
} 
