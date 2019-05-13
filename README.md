# File indexer

It works under Linux. It has not been tested on Windows.

# Compilation

It suffices to do:

```bash
mkdir bin obj
make
```

# Test

From one terminal, we should do, once compiled:

```bash
./bin/file-indexer 8000 .
```

This would mount this folder in the 8000 port. We have a test file named `requests` into this directory. To test the concurrent requests it contains, we should do, from another terminal:

```bash
cat requests | while read n; do printf "%q\n" "$n"; done | xargs -P 4 -I % bash -c %
```

This last test generates 4 concurrent requests, whose result will sometimes be unordered. An example:

```
<!DOCTYPE html> 
<html> 
<body> 
<pre> 
<!DOCTYPE html> 
<html> 
<body> 
<pre> 
#ifndef FILE__H
#define FILE__H

#include <string>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

using std::string;
using std::vector;

class File {
private: 
  string path;
  
public:  
  File(string path);
  
  bool is_file();
  
  bool is_folder();
  
  vector<string> list_files();
  
  File operator /(string file);
  
  string get_path() {
    return path;
  }
};

#endif
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
  int fd;
  string root_folder;
  const int BUFFER_SIZE = 1024;
  HttpResponses responses;
  
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

</pre> 
</body> 
<!DOCTYPE html> 
<html> 
  <body> 
    <h1> Not Found </h1> 
  </body> 

</pre> 
</body> 
</html> 
</html></html><!DOCTYPE html> 
<html> 
  <body> 
    <h1> Not Found </h1> 
  </body> 
</html>
```

![localhost-example](./imgs/localhost-example.png)

