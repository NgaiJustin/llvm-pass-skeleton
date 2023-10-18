#include <stdio.h>

#define N 3

int main() {
  int x = 0;

  for (int i = 0; i < N; i++) {
    int j = 2;
    x = x + j;
  }

  printf("x = %d\n", x);

  return 0;
}