#include <stdio.h>

#define N 11

int main() {
  int x = 0;
  for (int i = 0; i < N; i++) {
    x = x + i;
  }
  printf("x = %d\n", x);

  return 0;
}