#include <iostream>
#include <string>   
#include <string.h> // for strcmp
#include "client.h"
#include <cstdio>   // printf
#include <unistd.h> // sleep

using namespace std;


int main(int argc, char *argv[]) {
  cout << "INITIALIZING CLIENT" << endl;
  Client c = Client("http://localhost:3000/"); // initialize origin ip

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
        for (int i = 5; i > 0; i--) {
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
    printf("Specify --download, --upload or --sync\n");
  }

  return 0;
}
