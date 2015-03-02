#include <iostream>
#include <string> // for strcmp
#include <string.h> // for strcmp
#include "client.h"
#include "hash.h"
#include <cstdio> // printf
using namespace std;

int main(int argc, char *argv[]) {
  Client c = Client("http://localhost:5000/"); // initialize origin ip

  // baseDir set as arg2
  if (argc > 2)
    c.baseDir = argv[2];
  printf("Using %s as base directory", c.baseDir.c_str());

  if (argc > 1) {
    if (strcmp(argv[1], "--download") == 0) {
      printf("Download\n");
      c.syncDownload();
    } else if (strcmp(argv[1], "--upload") == 0) {
      printf("Upload\n");
      c.syncUpload();
    }
  } else {
    printf("Specify --download or --upload\n");
  }

  return 0;
}
