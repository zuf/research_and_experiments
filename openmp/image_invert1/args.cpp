#include <stdlib.h>
#include <stdio.h>

// argc - arguments count
// argv - array of arguments
// argv[0] - path to program itself
// argv[1] - 1st argument
int main(int argc, char *argv[])
{
  printf("Arguments count: %d\n", argc);

  int n = atoi(argv[1]);
  double d = atof(argv[2]);

  printf("n=%d\n", n);
  printf("n=%f\n", d);
}
