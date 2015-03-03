#include <iostream>
#include <string> // for strcmp
#include "fss.h"
using namespace std;

int main(int argc, char *argv[]) {
  printf("Running FSS\n");

  FSS fss;
  fss.listen();

  return 0;
}
