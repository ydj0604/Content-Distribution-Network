#include "client.h"
#include <iostream>
#include "dirent.h" // for file reading
using namespace std;

Client::Client() {
  syncDownload();
}
Client::~Client() {
}
void Client::syncDownload() {
  vector<string> fileNames = getListOfFilesFromDirectory();
  for (size_t i = 0; i < fileNames.size(); i++)
    cout << fileNames[i] << endl;

  /*
   getListOfFilesFromDirectory()
   compareListOfFiles()
   diff = list of files needed to dl
   foreach file
      downloadFile()
   */
}

void Client::syncUpload() {
  vector<string> fileNames = getListOfFilesFromDirectory();
  for (size_t i = 0; i < fileNames.size(); i++)
    cout << fileNames[i] << endl;

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
}

vector<string> Client::getListOfFilesFromDirectory() {
  cout << "Getting list of files from ./" << endl;
  vector<string> fileNames;

  DIR *dir = opendir(".");
  if (dir == NULL) {
    cout << "Could not open directory ./" << endl;
    return fileNames;
  }

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    // Skip hidden files
    if (ent->d_name[0] == '.')
      continue;

    // Convert to string object and add to return vector
    string fName = ent->d_name;
    fileNames.push_back(fName);
  }

  return fileNames;
}

void Client::getFileHash() {

}

void downloadFile(string fileName) {
  // request file from origin
  // wait for response from CDN node
}

void uploadFile(string fileName) {
  // upload file to cdn node
}
