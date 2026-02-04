/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c 
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>          // OpenMP library
#include <stdlib.h>       // atoi(), rand()
#include <limits.h>       // INT_MIN, INT_MAX
#include <stdio.h>

double start_time, end_time;

#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size; 
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;

  int max = INT_MIN;     // Start with smallest possible
  int min = INT_MAX;     // Start with largest possible  
  int max_row, max_col;
  int min_row, min_col;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  /* Safety Check */
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);    // setting the number of openmp threads 

  /* initialize the matrix (sequential)*/
  for (i = 0; i < size; i++) {
    //  printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      //	  printf(" %d", matrix[i][j]);
	  }
	  //	  printf(" ]\n");
  }

  start_time = omp_get_wtime();
  
  // parallel computing of sum, max, min, each thread works on a subset of rows 
  #pragma omp parallel for reduction(+:total) reduction(max:max) reduction(min:min) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if(matrix[i][j] > max){
        max = matrix[i][j];
      }
      if(matrix[i][j] < min){
        min = matrix[i][j];
      } 
    }

  // find the position of max and min, openmp reductions just find values, not the positions
  #pragma omp parallel for private(j)
  for(i = 0; i < size; i++){
    for(j = 0; j < size; j++){
      if(matrix[i][j] == max){
        // critical sections used, because multiple threads may find the smae max/min at the same time
        #pragma omp critical
        {
          max_row = i;
          max_col = j;
        }
      }
      if(matrix[i][j] == min){
        #pragma omp critical
        {
          min_row = i;
          min_col = j;
        }
      }
    }
  }

  // implicit barrier, automatic 


  end_time = omp_get_wtime();      // only runs after all parallel work is done 
 
  printf("the total is %d\n", total);
  printf("it took %g seconds\n", end_time - start_time);
  printf("the max is %d at position [%d, %d]\n", max, max_row, max_col);
  printf("the min is %d at position [%d, %d]\n", min, min_row, min_col);

}

