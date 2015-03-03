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

class ipToLatLng;

class Client {
public:
  Client();
  Client(string orig_ip);
  ~Client();
  void syncDownload();
  void syncUpload();
private:
  vector<FileInfo> compareListOfFiles(vector<FileInfo>& files, int type);
  vector<FileInfo> getListOfFilesFromDirectory(string subpath);
  void downloadFile(FileInfo f);
  void uploadFile(FileInfo f);
  bool isDir(string dirPath);

  ipToLatLng* ip_instance;
  string client_ip;
  double client_lat, client_lng;
  string m_orig_ip;
  string baseDir;
  
};

#endif
