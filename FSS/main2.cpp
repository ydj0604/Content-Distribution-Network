#include <iostream>
#include <string> // for strcmp
#include "fss.h"
using namespace std;

int main(int argc, char *argv[]) {
	string metaAddr = "localhost:4000";
	if(argc > 1) {
		 string temp(argv[1]);
		 metaAddr = temp;
	}
	FSS* fss = new FSS(metaAddr);
	fss->listen();
	return 0;
}
