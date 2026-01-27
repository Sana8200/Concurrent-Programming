/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers

*/
// 1. Header and Includes
/* Ensuring code is reentrant means that it's safe to be called by multiple threads at the same time
   Some libraries have thread-safe versions that are enabled by this flag */
#ifndef _REENTRANT   
#define _REENTRANT 
#endif 

#include <pthread.h>     // pthread library - thread creation, mutexes, condition variables
#include <stdlib.h>     // standard libraries - atoi() - rand() - etc. 
#include <stdio.h>      // Input / output - printf()
#include <stdbool.h>     // Boolean types
#include <time.h>        // time functions 
#include <sys/time.h>        // gettimeofday() for precise timing 

// 2. Constants
// Upper limits , the matrix can be at most 10000x10000 and we can have at most 10 worker threads
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

// 3. Global Synchronization Variables 
/* These are for the barrier synchronization 
   barrier - a mutex (mutual exclusion lock) to protect shared state
   go - a condition variable for threads to wait/signal 
   numArrived - counts how many threads have reached the barrier */
pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */ 
int numArrived = 0;       /* number who have arrived */

// 4. The Barrier Function 
/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);     // 1. Acquire lock
  numArrived++;     // 2. Increment arrival counter
  if (numArrived == numWorkers) {    // 3. Am i the last one?
    numArrived = 0;   // Reset counter for reuse
    pthread_cond_broadcast(&go);      // wake up all waiting threads
  } else
    pthread_cond_wait(&go, &barrier);   // 4. not last: wait for others
  pthread_mutex_unlock(&barrier);     // 5. Release the lock
}

// 5. Timer Function 
/* timer */
/* First call: Records start time and returns 0
   Subsequent calls: Returns elapsed time in seconds
*/
double read_timer() {
    static bool initialized = false;   // Initialization flag
    static struct timeval start;     // Start time
    struct timeval end;       // End time
    if( !initialized )        // First call
    {
        gettimeofday( &start, NULL );   // Record start time
        initialized = true;        // Set initialization flag
    }
    gettimeofday( &end, NULL );     // Record end time
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);      // Return elapsed time
}


// 6. Global Data 
double start_time, end_time; /* start and end times */
int size, stripSize;  /* assume size is multiple of numWorkers */   // Matrix size and rows per worker
int sums[MAXWORKERS]; /* partial sums */ // Arrays to store each worker's partial sum
int matrix[MAXSIZE][MAXSIZE]; /* matrix */ // The actual matrix

void *Worker(void *);

// 7. Main Function 
/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  // Variable Declarations
  int i, j;   // Loop counters
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;      // Thread attributes 
  pthread_t workerid[MAXWORKERS];     // Worker Thread IDs

  /* set global thread attributes */
  pthread_attr_init(&attr);     // Initialize Thread attributes 
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);    // Set thread scope
  // PTHREAD_SCOPE_SYSTEM means threads compete with all threads in the system of CPU time (true parallelism on multi-core)

  /* initialize mutex and condition variable */
  // Must initialize synchronization primitives before use
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;
  // Usage:  ./a.out 1000 4   → size=1000, numWorkers=4, stripSize=250

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = 1;//rand()%99;     // we'll change to rand() , now it's all ones 
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
  
  // Create the workers 
  /* do the parallel work: create the workers */
  /* pthread_create parameters:
   * thread ID, thread attributes, start routine, argument
   * &workeridp[l] - where to store the thread ID
   * &attr - thread attributes
   * Worker - the function this thread will execute
   * (void *) l - argument passed to the thread (worker ID)
   */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)   
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  pthread_exit(NULL);    // Main thread exists, but process continues until all threads done 
}

// 8. Worker Function 
/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;    // get my thread ID (0, 1, 2, ...)
  int total, i, j, first, last;   

#ifdef DEBUG
  printf("worker %d (pthread id %d) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;    // first row
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);    // last row
  /* For the last worker, we make sure to include any remaining rows if size isn't perfectly divisible*/
  
  /* sum values in my strip */
  total = 0;
  for (i = first; i <= last; i++)
    for (j = 0; j < size; j++)
      total += matrix[i][j];
  sums[myid] = total;     // Store my partial results 
  // Each worker stores result in sums[myid] - no conflict because each uses different index
  Barrier();   // Wait for all workers to finish their sums 
  if (myid == 0) {      // Only worker 0 does the final computations
    total = 0;
    for (i = 0; i < numWorkers; i++)
      total += sums[i];
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total);
    printf("The execution time is %g sec\n", end_time - start_time);
  }
}



