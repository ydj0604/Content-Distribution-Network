#include <iostream>
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include "hash.h"
using namespace std;

/*
 Hashing uses sha256.
 Typically this is done using openssl's libraries, but OS X has deprecated
 these functions (instead uses CC_Crypto). Because development is on OS X,
 rather than spend a lot of energy on this,  hashing is performed by making
 a system call and returning the output.
 */

string exec(const char* cmd) {
    FILE* pipe = popen(cmd, "r");
    if (!pipe)
      return "ERROR";

    char buffer[128];
    string result = "";
    while(!feof(pipe)) {
      if(fgets(buffer, 128, pipe) != NULL)
        result += buffer;
    }
    pclose(pipe);

    return result;
}

string hashFile(string relativeFilePath) {
  // This class project is not dealing with injection concerns
  string cmd = "shasum -a 256 -p " + relativeFilePath + " | grep -o '.* ' | tr -d ' \n'";
  return exec(cmd.c_str());
}
