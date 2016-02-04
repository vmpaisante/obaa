#include <stdlib.h>
#include <stdio.h>

int main (int argc, char** argv) {
  char* v = (char*) malloc(argc*sizeof(char));
  v[1] = 0;
  return v[1];
}
