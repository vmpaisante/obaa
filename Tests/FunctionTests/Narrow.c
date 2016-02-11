#include <stdlib.h>

int main (int argc, char** argv) {
  char* v = malloc (10);
  char* e = v + 8;
  char* i;
  for (i = v; i < e; i++) {
    *i = argc;
  }
  
  return i;
}
