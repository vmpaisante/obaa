#include <stdlib.h>
#include <stdio.h>

int main (int argc, char** argv) {
  int* v = (int*) malloc(argc*sizeof(int));
  v[0] = 0;
  return v[0];
}
