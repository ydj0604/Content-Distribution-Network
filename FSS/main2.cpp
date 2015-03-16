#include <iostream>
#include <string> // for strcmp
#include "fss.h"
using namespace std;

int main(int argc, char *argv[]) {
	string metaAddr = "localhost:4000", fssAddr = "localhost:5000";

  cout << "INITIALIZING FSS" << endl;

  if (argc > 2) {
	  string temp(argv[1]);
	  fssAddr = temp;
	  string temp2(argv[2]);
	  metaAddr = temp2;
	  FSS* fss = new FSS(metaAddr, fssAddr);
	  fss->listen();
  }
  return 0;
}
