#include <stdlib.h>
#include <stdio.h>

void printFifths(char* a, char* b) {
  char a5 = a[4];
  char b5 = b[4];
  printf("%c and %c", a5, b5);
}

int main (int argc, char** argv) {
  char* v1 = (char*) malloc (5);
  char* v2 = (char*) malloc (5);
  printFifths(v1, v2);
  return 0;
}
