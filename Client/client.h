#ifndef CDN_CLIENT_H
#define CDN_CLIENT_H

#include <string>
#include <vector>
using namespace std;

struct FileInfo
{
  string name;
  string hash;
};

class Client {
public:
  Client();
  ~Client();
  void syncDownload();
  void syncUpload();
private:
  vector<string> compareListOfFiles();
  vector<FileInfo> getListOfFilesFromDirectory();
  void downloadFile(string fileName);
  void uploadFile(string fileName);
};

#endif
