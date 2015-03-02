#include "client.h"
#include <iostream>
#include "dirent.h" // for file reading
#include "hash.h"
#include <cstdio> // for printf

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams


using namespace std;

FileInfo newFileInfo(string name, string hash, string cdnAddr) {
  FileInfo f = FileInfo();
  f.name = name;
  f.hash = hash;
  f.cdnAddr = cdnAddr;
  return f;
}

void printFileInfo(FileInfo f) {
  printf("%s %s %s\n", f.name.c_str(), f.hash.c_str(), f.cdnAddr.c_str());
}

Client::Client() {
  baseDir = "./";
}

Client::~Client() {
}

void Client::syncDownload() {
  // Get list of files in directory
  vector<FileInfo> files = getListOfFilesFromDirectory();
  for (size_t i = 0; i < files.size(); i++)
    printFileInfo(files[i]);

  // Compare with origin server
  vector<FileInfo> diffFiles = compareListOfFiles(files);

  // For each file that needs to be updated, download
  for(size_t i = 0; i < diffFiles.size();i ++)
    downloadFile(diffFiles[i]);
}

void Client::syncUpload() {
  // Get list of files in directory
  vector<FileInfo> files = getListOfFilesFromDirectory();
  for (size_t i = 0; i < files.size(); i++)
    printFileInfo(files[i]);

  // Compare with origin server
  vector<FileInfo> diffFiles = compareListOfFiles(files);

  // For each file that needs to be updated, upload
  for(size_t i = 0; i < diffFiles.size();i ++)
    uploadFile(diffFiles[i]);
}

vector<FileInfo> Client::compareListOfFiles(vector<FileInfo>& files) {
  // Upload list of file / hashes
  // return list of fileNames that need to be dl'd / uploaded
  vector<string> diff;

  // upload files
  // diff = what server returns as files needed to download

  // return diff;
  return files;
}

vector<FileInfo> Client::getListOfFilesFromDirectory() {
  cout << "Getting list of files from " << baseDir << endl;
  vector<FileInfo> files;

  DIR *dir = opendir(baseDir.c_str());
  if (dir == NULL) {
    cout << "Could not open directory " << baseDir << endl;
    return files;
  }

  struct dirent *ent;
  while ((ent = readdir(dir)) != NULL) {
    // Skip hidden files
    if (ent->d_name[0] == '.')
      continue;

    // Convert to string object and add to return vector
    FileInfo f = newFileInfo(ent->d_name, hashFile(baseDir+ent->d_name));
    files.push_back(f);
  }

  return files;
}

void Client::downloadFile(FileInfo f) {
  // directly download file from cdn
  printf("Downloading file... ");
  printFileInfo(f);

  // request file f communication
  string cdn_address = "http://" + f.cdnAddr + "/";
  http_client cdn_client = http_client("http://localhost:5000/get");
  
  // Make request
  http_response response;
  try {
    response = cdn_client.request(methods::GET, f.name).get();
  } catch (const std::exception& e) {
    printf("ERROR when downloading, %s\n", e.what());
  }
  
  if (response.status_code() == status_codes::OK) {
    printf("OK, saving...\n");

    string contents = response.extract_string().get();
    cout << contents << endl;

    // Write to file
    ofstream saveFile;
    saveFile.open(baseDir + f.name);
    saveFile << contents;
    saveFile.close();
  } else {
    printf("FAILED TO DOWNLOAD\n");
  }

}

void Client::uploadFile(FileInfo f) {
  printf("Uploading file... ");
  printFileInfo(f);
  // upload file to cdn node
}
