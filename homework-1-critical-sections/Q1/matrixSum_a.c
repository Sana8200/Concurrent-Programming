/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

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
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);    // acquire the barrier lock
  numArrived++;    // increment the number of arrived workers
  if (numArrived == numWorkers) {    // all workers have arrived
    numArrived = 0;    // reset the counter
    pthread_cond_broadcast(&go);    // wake up all waiting workers
  } else
    pthread_cond_wait(&go, &barrier);    // wait for the barrier
  pthread_mutex_unlock(&barrier);  // release the barrier lock
}

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
int sums[MAXWORKERS]; /* partial sums */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

int max_val[MAXWORKERS];       // maximum numbers from each worker
int max_val_col[MAXWORKERS];    // column indices of maximum numbers
int max_val_row[MAXWORKERS];    // row indices of maximum numbers
int min_val[MAXWORKERS];      // minimum numbers from each worker
int min_val_col[MAXWORKERS];      // column indices of minimum numbers
int min_val_row[MAXWORKERS];      // row indices of minimum numbers

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
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;   // size of the matrix is from command line or default
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;  // strip of rows divided among workers equally

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand() % 5;  
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
  pthread_exit(NULL);
}

// each worker thread executes this function
/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
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
  sums[myid] = total;
  max_val[myid] = max_found;
  max_val_col[myid] = max_col_found;
  max_val_row[myid] = max_row_found;
  min_val[myid] = min_found;
  min_val_col[myid] = min_col_found;
  min_val_row[myid] = min_row_found;

  Barrier();

  // Last worker computes sums, max and min
  if (myid == 0) {
    total = 0;
    for (i = 0; i < numWorkers; i++){
        total += sums[i];
    }
    max_found = max_val[0];
    max_row_found = max_val_row[0];
    max_col_found = max_val_col[0];
    min_found = min_val[0];
    min_row_found = min_val_row[0];
    min_col_found = min_val_col[0];
    for(i = 1; i < numWorkers; i++){
        if(max_val[i] > max_found){
            max_found = max_val[i];
            max_col_found = max_val_col[i];
            max_row_found = max_val_row[i];
        }
        if(min_val[i] < min_found){
            min_found = min_val[i];
            min_col_found = min_val_col[i];
            min_row_found = min_val_row[i];
        }
      }
    
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total);
    printf("The execution time is %g sec\n", end_time - start_time);
    printf("The maximum value is %d at position [%d, %d]\n", max_found, max_row_found, max_col_found);
    printf("The minimum value is %d at position [%d, %d]\n", min_found, min_row_found, min_col_found);
  }
}