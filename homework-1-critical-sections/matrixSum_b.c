/* matrix summation using pthreads

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
#ifndef _REENTRANT    // reentrant makes sure the program is thread-safe
#define _REENTRANT 
#endif 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>    // for INT_MAX and INT_MIN 
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

// mutex locks for min, max, sum (global shared variables)
pthread_mutex_t max_lock;
pthread_mutex_t min_lock;
pthread_mutex_t sum_lock;

//pthread_mutex_t barrier;  /* mutex lock for the barrier */
//pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
//int numArrived = 0;       /* number who have arrived */

#ifdef USE_BARRIER
/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);   
  numArrived++;   
  if (numArrived == numWorkers) {    
    numArrived = 0;   
    pthread_cond_broadcast(&go);    
  } else
    pthread_cond_wait(&go, &barrier);    
  pthread_mutex_unlock(&barrier);  
}
#endif

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */
//int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

int total_sum = 0;      // All workers add here 
int max = INT_MIN;      // all threads update this 
int min = INT_MAX;
int max_row = -1;
int max_col = -1;
int min_row = -1;
int min_col = -1;

void *Worker(void *);


/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  //pthread_mutex_init(&barrier, NULL);
  //pthread_cond_init(&go, NULL);
  /* initilaizing mutex locks for sum, min, max */
  pthread_mutex_init(&sum_lock, NULL);
  pthread_mutex_init(&min_lock, NULL);
  pthread_mutex_init(&max_lock, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;   // size of the matrix is from command line or default
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;  // strip of rows divided among workers equally

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand() % 1000;   
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);  // creating the workers 
  //pthread_exit(NULL);  // main thread exits, others continure until finished

  // Since we don't want the main thread to exit before the workers are done,
  // we need to wait for all of them to finish.
  for (l = 0; l < numWorkers; l++){
    pthread_join(workerid[l], NULL);
  }

  /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total_sum);
    printf("The execution time is %g sec\n", end_time - start_time);
    printf("The maximum value is %d at position [%d, %d]\n", max, max_row, max_col);
    printf("The minimum value is %d at position [%d, %d]\n", min, min_row, min_col);
    
    return 0;
}




// each worker thread executes this function
void *Worker(void *arg) {
  long myid = (long) arg;  // getting my thread id - using long because in pthread we cast id to void, now cast back to long
  int total, i, j, first, last;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  int max_found = matrix[first][0];
  int min_found = matrix[first][0];
  int max_col_found = 0;
  int max_row_found = first;
  int min_col_found = 0;
  int min_row_found = first;


  /* sum values in my strip */ 
  // Changing worker function so in addition to sum find max and min in each strip as well
  total = 0;
  for (i = first; i <= last; i++){
    for (j = 0; j < size; j++) {
      total += matrix[i][j];     
      if(matrix[i][j] > max_found){
        max_found = matrix[i][j];
        max_col_found = j;
        max_row_found = i;
      }
      if(matrix[i][j] < min_found){
        min_found = matrix[i][j];
        min_col_found = j;
        min_row_found = i;  
      }
    }
  }
  pthread_mutex_lock(&sum_lock);
  total_sum += total;
  pthread_mutex_unlock(&sum_lock);
  pthread_mutex_lock(&max_lock);
  if(max_found > max){
    max = max_found;
    max_col = max_col_found;
    max_row = max_row_found;
  }
  pthread_mutex_unlock(&max_lock);
  pthread_mutex_lock(&min_lock);
  if(min_found < min){
    min = min_found;
    min_col = min_col_found;
    min_row = min_row_found;
  }
  pthread_mutex_unlock(&min_lock);

  //Barrier();

  /* No need for barrier and last thread doing the job*/
   return NULL;
}