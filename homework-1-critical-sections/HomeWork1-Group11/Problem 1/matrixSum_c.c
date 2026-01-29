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

int nextRow = 0;    // next row to be taken and processed
pthread_mutex_t nextRow_lock;       // mutex lock for nextRow

// mutex locks for min, max, sum (global shared variables)
pthread_mutex_t max_lock;
pthread_mutex_t min_lock;
pthread_mutex_t sum_lock;

int numWorkers;           /* number of workers */ 

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
int size;  /* assume size is multiple of numWorkers */
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

  /* initilaizing mutex locks for sum, min, max */
  pthread_mutex_init(&sum_lock, NULL);
  pthread_mutex_init(&min_lock, NULL);
  pthread_mutex_init(&max_lock, NULL);

  pthread_mutex_init(&nextRow_lock, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;  
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  //stripSize = size/numWorkers;  

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
  for (l = 0; l < numWorkers; l++){
    pthread_create(&workerid[l], &attr, Worker, (void *) l);  // creating the workers 
  }
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
  long myid = (long) arg;   // for debug part 
  int i;

  int my_total_sum = 0;
  int my_max = INT_MIN;     // any possible value bigger than this
  int my_min = INT_MAX;
  int my_max_row, my_max_col, my_min_row, my_min_col;
  int row;

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  //first = myid*stripSize;
  //last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  while(1){
    pthread_mutex_lock(&nextRow_lock);
    if(nextRow >= size){     // checking if there are no more rows
      pthread_mutex_unlock(&nextRow_lock);
      break;
    }else{
      row = nextRow;
      nextRow++;
      pthread_mutex_unlock(&nextRow_lock); 
    }

    for(i=0; i < size; i++){
      my_total_sum += matrix[row][i];
      if(matrix[row][i] > my_max){
        my_max = matrix[row][i];
        my_max_row = row;
        my_max_col = i;
      }
      if(matrix[row][i] < my_min){
        my_min = matrix[row][i];
        my_min_row = row;
        my_min_col = i;
      }
    }
  }

  
  pthread_mutex_lock(&sum_lock);
  total_sum += my_total_sum;
  pthread_mutex_unlock(&sum_lock);
  pthread_mutex_lock(&max_lock);
  if(my_max > max){
    max = my_max;
    max_col = my_max_col;
    max_row = my_max_row;
  }
  pthread_mutex_unlock(&max_lock);
  pthread_mutex_lock(&min_lock);
  if(my_min < min){
    min = my_min;
    min_col = my_min_col;
    min_row = my_min_row;
  }
  pthread_mutex_unlock(&min_lock);

   return NULL;
}