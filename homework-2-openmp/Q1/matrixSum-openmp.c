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


/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;

  int max = INT_MIN;     // Start with smallest possible
  int min = INT_MAX;     // Start with largest possible  
   int max_row = 0, max_col = 0, min_row = 0, min_col = 0;


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
      matrix[i][j] = rand()% 1000;
      //	  printf(" %d", matrix[i][j]);
	  }
	  //	  printf(" ]\n");
  }

  start_time = omp_get_wtime();
  
  // parallel computing of sum, max, min, each thread works on a subset of rows 
#pragma omp parallel for reduction(+:total) private(j)
  for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];

      // entering critical section if found a max and min (using double check)
      if (max < matrix[i][j]) {
#pragma omp critical        
				{
					if (max < matrix[i][j]) {
						max = matrix[i][j];
						max_row = i;
						max_col = j;
					}
				}
			}

			if (min > matrix[i][j]) {
#pragma omp critical
				{
					if (min > matrix[i][j]) {
						min = matrix[i][j];
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

  return 0;
}