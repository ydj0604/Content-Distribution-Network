#ifndef CDN_CLIENT_H
#define CDN_CLIENT_H

#include <string>
#include <vector>
#include <iostream>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include <unordered_map>

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
  Client(string orig_ip, string loc="la");
  ~Client();
  void syncDownload();
  void syncUpload();
  void autoSync(bool isFirstRun);
  string baseDir;

private:
  void initClient(string loc);
  void syncExplicit(int downloadOrUpload);

  vector<FileInfo> compareListOfFiles_explicit(vector<FileInfo>& files, int type);
  void compareListOfFiles_sync(vector<FileInfo>& files, vector<FileInfo>& uploadFileList, vector<FileInfo>& downloadFileList);
  vector<FileInfo> getListOfFilesFromDirectory(string subpath);
  void downloadFile(FileInfo f);
  void uploadFile(FileInfo f);
  bool isDir(string dirPath);

  // For delete/new
  vector<FileInfo> postSyncFileList;
  vector<FileInfo> deletedFiles;
  bool needsSyncUpload = false;
  bool fileListContains(string filePath, vector<FileInfo>& fileList);
  void compareLocalFileLists(vector<FileInfo>&postSyncList, vector<FileInfo>&current);


  ipToLatLng* ip_instance;
  string client_ip;
  double client_lat, client_lng;
  string m_orig_ip;
  unordered_map<string, string> fileToTimeStamp;  // filename mapped to its timestamp
  
};

#endif
