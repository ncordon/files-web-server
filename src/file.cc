#include "file.h"

 
File::File(string path) {
  if (!path.empty() && path.back() == '/')
    path.pop_back();
    
  this->path = path;
}

bool File::is_file() {
  struct stat stat_result;
  stat(path.c_str(), &stat_result);

  return S_ISREG(stat_result.st_mode);
}

bool File::is_folder() {
  struct stat stat_result;
  stat(path.c_str(), &stat_result);
    
  return S_ISDIR(stat_result.st_mode);
}

vector<string> File::list_files() {
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

File File::operator /(string file) {
  string sanitized_path = path;

  if (!path.empty() && path.back() == '/')
    sanitized_path.pop_back();

  if (!file.empty() && file.front() == '/')
    file.erase(file.begin());
    
  return File(sanitized_path + "/" + file);
}
