#include <stdlib.h>
#include <stdio.h>

int* retSized(int s) {
  int* r = (int*) malloc(s*sizeof(int));
  return r;
}

int main (int argc, char** argv) {
  int v = retSized(argc);
  
  return v;
}
