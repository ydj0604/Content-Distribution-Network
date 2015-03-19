#include <iostream>
#include <string>   
#include <string.h> // for strcmp
#include "client.h"
#include <cstdio>   // printf
#include <unistd.h> // sleep

using namespace std;


int main(int argc, char *argv[]) {
  cout << "INITIALIZING CLIENT" << endl;

  string originIp = "localhost:3000", loc="la";

  if(argc > 4)
	  loc = argv[4];

  if (argc > 3)
    originIp = argv[3];

  cout << "origin ip: " << "http://"+originIp << endl;
  Client c = Client("http://"+originIp, loc); // initialize origin ip

  cout << endl;

  // baseDir set as arg2
  if (argc > 2)
    c.baseDir = argv[2];
  printf("Base Dir: %s\n", c.baseDir.c_str());


  if (argc > 1) {
    if (strcmp(argv[1], "--download") == 0) {
      printf("Download\n");
      c.syncDownload();
    } else if (strcmp(argv[1], "--upload") == 0) {
      printf("Upload\n");
      c.syncUpload();
    } else if (strcmp(argv[1], "--sync") == 0) {
      
      bool isFirstRun = true;
      while (1) {

        printf("\n  Autosyncing.....");
        
        // sleep for 10 seconds until next syncing
        for (int i = 10; i > 0; i--) {
          cout << i << " ";
          fflush(stdout);
          sleep(1);
        }
        cout << endl << endl;

        // call the autoSync
        c.autoSync(isFirstRun);

        // set it to false to avoid explicit download in the next iteration
        isFirstRun = false;

      }
    }
  } else {
    printf("Usage: ./client [--download,--upload,--sync] [dir] [meta address]\n");
  }

  return 0;
}
