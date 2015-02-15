#include "client.h"
#include <iostream>
#include "dirent.h" // for file reading
#include "hash.h"
using namespace std;

Client::Client() {
  syncDownload();
}
Client::~Client() {
}
void Client::syncDownload() {
  vector<FileInfo> files = getListOfFilesFromDirectory();
  for (size_t i = 0; i < files.size(); i++)
    cout << files[i].name << " " << files[i].hash << endl;

  /*
   getListOfFilesFromDirectory()
   compareListOfFiles()
   diff = list of files needed to dl
   foreach file
      downloadFile()
   */
}

void Client::syncUpload() {
  /*
  vector<string> fileNames = getListOfFilesFromDirectory();
  for (size_t i = 0; i < fileNames.size(); i++)
    cout << fileNames[i] << endl;
  */
  /*
   getListOfFilesFromDirectory()
   compareListOfFiles()
   diff = list of files needed to upload
   foreach file
      uploadFile()
   */
}

vector<string> Client::compareListOfFiles() {
  // Upload list of file / hashes
  // return list of fileNames that need to be dl'd / uploaded
  vector<string> files;
  return files;
}

vector<FileInfo> Client::getListOfFilesFromDirectory() {
  cout << "Getting list of files from ./" << endl;
  vector<FileInfo> files;

  DIR *dir = opendir(".");
  if (dir == NULL) {
    cout << "Could not open directory ./" << endl;
    return files;
  }

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    // Skip hidden files
    if (ent->d_name[0] == '.')
      continue;

    // Convert to string object and add to return vector
    FileInfo f = FileInfo();
    f.name = ent->d_name;
    f.hash = hashFile(f.name);
    files.push_back(f);
  }

  return files;
}

void downloadFile(string fileName) {
  // request file from origin
  // wait for response from CDN node
}

void uploadFile(string fileName) {
  // upload file to cdn node
}
