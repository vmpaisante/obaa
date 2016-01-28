#include <stdlib.h>

int main (int argc, char** argv) {
  char* v = (char*) malloc(10);
  char* e = v + 10;
  char* i;
  for(i = v; i < e; i++) {
    *i = 'x';
  }
  
  return i;
}
