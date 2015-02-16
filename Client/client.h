#ifndef CDN_CLIENT_H
#define CDN_CLIENT_H

#include <string>
#include <vector>
using namespace std;

struct FileInfo
{
  string name;
  string hash;
  string cdnAddr;
};
FileInfo newFileInfo(string name, string hash, string cdnAddr = "0.0.0.0");
void printFileInfo(FileInfo f);

class Client {
public:
  Client();
  ~Client();
  void syncDownload();
  void syncUpload();
private:
  vector<FileInfo> compareListOfFiles(vector<FileInfo>& files);
  vector<FileInfo> getListOfFilesFromDirectory();
  void downloadFile(FileInfo f);
  void uploadFile(FileInfo f);
};

#endif
