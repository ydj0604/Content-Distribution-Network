#include <iostream>
#include "client.h"
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* system, NULL, EXIT_FAILURE */
#include "hash.h"
using namespace std;

int main(int argc, char *argv[]) {
  Client c;
  cout << hashFile("main.cpp") << endl; 
  return 0;
}
