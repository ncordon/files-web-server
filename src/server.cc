#include "server.h"


string HttpResponses::NOT_FOUND() {
  string result;

  result  = "HTTP/1.1 404 Not Found \n";
  result += "Content-Type: text/html; charset=utf-8 \n";
  result += "\n";
  result += "<!DOCTYPE html> \n";
  result += "<html> \n";
  result += "  <body> \n";
  result += "    <h1> Not Found </h1> \n";
  result += "  </body> \n";
  result += "</html> \n";

  return result;
}


string HttpResponses::OK(string msg) {
  string result;
  result  = "HTTP/1.1 200 OK \n";
  result += "Content-Type: text/html; charset=utf-8 \n";
  result += "\n";
  result += msg;
  return result;
}


Request::Request(string root_folder, int fd) {
  this->root_folder = root_folder;
  this->fd = fd;
  this->responses = HttpResponses();
}


void Request::answer() {
  string request;
  string http_method;
  string msg;
  char buffer[BUFFER_SIZE];
  int num_read;
  int total_read = 0;
  int i = 0;

  // Sleep for 100 microseconds if the descriptor is not ready to be read
  while ((num_read = read(fd , buffer, BUFFER_SIZE)) <= 0)
    usleep(100);

  // Read the request
  while (num_read > 0) {
    for (int i = 0; i < num_read; ++i)
      request += buffer[i];

    total_read += num_read;
    num_read = read(fd , buffer, BUFFER_SIZE);
  }

  // Read the method from the request
  while (i < total_read && request[i] != ' ') {
    http_method += request[i];
    ++i;
  }
  
  if (http_method == "GET") {
    ++i;
    string path;
    
    while (i < total_read && buffer[i] != ' ') {
      path += buffer[i];
      ++i;
    }
    
    File current = File(root_folder) / path;
      
    if (current.is_folder()) {
      vector<string> files = current.list_files();
      string listing = "<ul> \n";

      // Generate the list of files
      for (auto f : files) {
        File child = File(path) / f;
        listing += "<li><a href=\"" + child.get_path() + "\">" + f + "</a></li> \n";
      }

      listing += "</ul> \n";      
      msg = responses.OK(
          "<!DOCTYPE html> \n<html> \n<body> \n" + listing + "</body> </html>");
      
      send(fd, msg.c_str(), msg.size(), 0);
    } else if (current.is_file()) {
      msg = responses.OK("<!DOCTYPE html> \n<html> \n<body> \n<pre> \n");
      send(fd, msg.c_str(), msg.size(), 0);
      char read_buffer[BUFFER_SIZE];
        
      ifstream is(current.get_path());

      // Read the content of the file
      // We need to buffer it, since if the file
      // was too heavy, it would not fit into memory
      while (is) {
        is.read(read_buffer, BUFFER_SIZE);
        send(fd, read_buffer, is.gcount(), 0);
      }

      string end_msg = "\n</pre> \n</body> \n</html>";
      send(fd, end_msg.c_str(), end_msg.size(), 0);
    } else {
      msg = responses.NOT_FOUND();
      send(fd, msg.c_str(), msg.size(), 0);
    }
  } else {
    msg = responses.NOT_FOUND();
    send(fd, msg.c_str(), msg.size(), 0);
  }

  close(fd);
}


FileServer::FileServer(int port, string root_folder) {
  int opt = 1;
  
  // Create file descriptor for the socket
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);
  
  struct sockaddr_in address;
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

  this->root_folder = root_folder;
}


Request FileServer::await_request() {
  struct sockaddr_in their_addr;
  socklen_t address_size = sizeof(their_addr);
  int fd = accept(socket_fd, (struct sockaddr*) &their_addr, &address_size);
  
  // Make fd non blocking for reads
  int flags = fcntl(fd, F_GETFL, 0);
  fcntl(fd, F_SETFL, flags | O_NONBLOCK);
  
  return Request(root_folder, fd);
}
