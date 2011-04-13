#include <stdio.h>
#include <omp.h>
#include <math.h>
#include <stdlib.h>

#define THREADS_NUMBER 16
#define LOOP_TIMES 10000

int main(int argc, char *argv[])
{
  #ifdef _OPENMP
    printf("I am compiled with OpenMP support!\n");
    printf("Run with %d threads.\n", THREADS_NUMBER);
    omp_set_num_threads(THREADS_NUMBER);
  #endif

  int i,n;
  double *sum = (double*)malloc(sizeof(double)*THREADS_NUMBER);
  #ifdef _OPENMP
    double start=omp_get_wtime();
  #endif
  #pragma omp parallel shared(sum) private(i,n)
  #pragma omp for
  for(i=1; i<LOOP_TIMES; i++)
  {
    for(n=1; n<LOOP_TIMES; n++)
    {
      #ifdef _OPENMP
        sum[omp_get_thread_num()] += (sqrt(i+n) / sin(sqrt(n*i)) * cos(sqrt(n+2*i)) + sqrt(sqrt(i+n+sin((i+n)*n))));
      #else
        sum[0] += (sqrt(i+n) / sin(sqrt(n*i)) * cos(sqrt(n+2*i)) + sqrt(sqrt(i+n+sin((i+n)*n))));
      #endif
    }
  }
  #ifdef _OPENMP
    printf("Work time: %f\n", omp_get_wtime()-start);
  #endif
  double final_sum=0;
  for(i=0; i<THREADS_NUMBER; i++)
  {
    final_sum += sum[i];
  }
  printf("Sum: %f\n", final_sum);
  free(sum);

  return 0;
}
