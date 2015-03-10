#ifndef CDN_CLIENT_H
#define CDN_CLIENT_H

#include <string>
#include <vector>

using namespace std;

struct FileInfo
{
  string name;
  string hash;
  string timestamp;
  string cdnAddr;
};

FileInfo newFileInfo(string name, string hash, string timestamp, string cdnAddr = "0.0.0.0");
void printFileInfo(FileInfo f);

class ipToLatLng;

class Client {
public:
  Client();
  Client(string orig_ip);
  ~Client();
  void syncDownload();
  void syncUpload();
  string baseDir;

private:
  vector<FileInfo> compareListOfFiles_explicit(vector<FileInfo>& files, int type);
  void compareListOfFiles_sync(vector<FileInfo>& files, vector<FileInfo>& uploadFileList, vector<FileInfo>& downloadFileList);
  vector<FileInfo> getListOfFilesFromDirectory(string subpath);
  void downloadFile(FileInfo f);
  void uploadFile(FileInfo f);
  bool isDir(string dirPath);

  ipToLatLng* ip_instance;
  string client_ip;
  double client_lat, client_lng;
  string m_orig_ip;
  
};

#endif
