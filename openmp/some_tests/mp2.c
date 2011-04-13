#include <stdio.h>
#include <omp.h>

int main(int argc, char *argv[])
{

  #pragma omp parallel
  printf("I am thread %d\n", omp_get_thread_num());

  #ifdef _OPENMP
    printf("I am compiled with OpenMP support!\n");
  #endif

  return 0;
}
