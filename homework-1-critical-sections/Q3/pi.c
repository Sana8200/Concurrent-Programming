/* Compute Pi using Adaptive Quadrature
   
   f(x) = sqrt(1 - x^2) is a quarter circle
   Area from 0 to 1 = pi/4
   So pi = 4 * area

   usage: ./pi numWorkers epsilon
   example: ./pi 4 0.0000001
*/

#ifndef _REENTRANT
#define _REENTRANT
#endif

#define _USE_MATH_DEFINES  // For M_PI 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <sys/time.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAXWORKERS 10

int numWorkers;
double epsilon;

double partial_areas[MAXWORKERS];  // Each worker stores its result here

pthread_mutex_t barrier_lock;
pthread_cond_t go;
int numArrived = 0;

/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if (!initialized) {
        gettimeofday(&start, NULL);
        initialized = true;
    }
    gettimeofday(&end, NULL);
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

/* Barrier for synchronization */
void Barrier() {
    pthread_mutex_lock(&barrier_lock);
    numArrived++;
    if (numArrived == numWorkers) {
        numArrived = 0;
        pthread_cond_broadcast(&go);
    } else {
        pthread_cond_wait(&go, &barrier_lock);
    }
    pthread_mutex_unlock(&barrier_lock);
}

/* The function f(x) = sqrt(1 - x^2) - quarter circle */
double f(double x) {
    return sqrt(1.0 - x * x);
}

/* Trapezoid area between two points */
double trapezoid(double left, double right) {
    return (f(left) + f(right)) * (right - left) / 2.0;
}

/* Adaptive quadrature - recursively refines until accurate enough */
double adaptive_quadrature(double left, double right, double whole_area) {
    double mid = (left + right) / 2.0;
    double left_area = trapezoid(left, mid);
    double right_area = trapezoid(mid, right);
    double sum = left_area + right_area;
    
    // If the two halves are close enough to the whole, we're done
    // Using epsilon scaled by interval width for better precision
    if (fabs(sum - whole_area) < epsilon * (right - left)) {
        return sum;
    }
    
    // Otherwise, recurse on both halves
    return adaptive_quadrature(left, mid, left_area) + 
           adaptive_quadrature(mid, right, right_area);
}

void *Worker(void *arg);

int main(int argc, char *argv[]) {
    long l;
    pthread_attr_t attr;
    pthread_t workerid[MAXWORKERS];
    double start_time, end_time;
    
    /* Set default values */
    numWorkers = (argc > 1) ? atoi(argv[1]) : 4;
    epsilon = (argc > 2) ? atof(argv[2]) : 0.0000001;
    
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
    
    printf("Computing pi with %d workers and epsilon = %g\n", numWorkers, epsilon);
    
    /* Initialize thread attributes */
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    
    /* Initialize barrier */
    pthread_mutex_init(&barrier_lock, NULL);
    pthread_cond_init(&go, NULL);
    
    /* Start timer and create workers */
    start_time = read_timer();
    
    for (l = 0; l < numWorkers; l++) {
        pthread_create(&workerid[l], &attr, Worker, (void *) l);
    }
    
    /* Wait for all workers */
    for (l = 0; l < numWorkers; l++) {
        pthread_join(workerid[l], NULL);
    }
    
    end_time = read_timer();
    printf("Execution time = %g sec\n", end_time - start_time);
    
    /* Cleanup */
    pthread_mutex_destroy(&barrier_lock);
    pthread_cond_destroy(&go);
    
    return 0;
}


/* Worker function */
void *Worker(void *arg) {
    long myid = (long) arg;
    
    // Each worker gets an equal interval of [0, 1]
    double interval_size = 1.0 / numWorkers;
    double left = myid * interval_size;
    double right = (myid == numWorkers - 1) ? 1.0 : (myid + 1) * interval_size;
    
    // Compute area of my interval using adaptive quadrature
    double whole = trapezoid(left, right);
    double my_area = adaptive_quadrature(left, right, whole);
    
    // Store my result
    partial_areas[myid] = my_area;
    
    Barrier();
    
    // Worker 0 computes final result
    if (myid == 0) {
        double total_area = 0.0;
        for (int i = 0; i < numWorkers; i++) {
            total_area += partial_areas[i];
        }
        double pi = 4.0 * total_area;
        printf("Computed pi = %.15f\n", pi);
        printf("Actual pi   = %.15f\n", M_PI);
        printf("Difference  = %.15f\n", fabs(pi - M_PI));
    }  
    return NULL;
}
