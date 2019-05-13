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
