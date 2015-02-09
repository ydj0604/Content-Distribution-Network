#ifndef CDN_CLIENT_H
#define CDN_CLIENT_H

#include <vector>
using namespace std;

class Client {
public:
  Client();
  ~Client();
  void syncDownload();
  void syncUpload();
private:
  vector<string> compareListOfFiles();
  vector<string> getListOfFilesFromDirectory();
  void getFileHash();
  void downloadFile(string fileName);
  void uploadFile(string fileName);
};
#endif
