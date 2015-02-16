#include <iostream>
#include <string> // for strcmp
#include "client.h"
#include "hash.h"
using namespace std;

int main(int argc, char *argv[]) {
  Client c;

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
