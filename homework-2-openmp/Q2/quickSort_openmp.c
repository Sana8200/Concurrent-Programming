#include <stdio.h>
#include <omp.h>
#include <stdlib.h>

#define MAXSIZE 10000000   /* maximum array size */
#define MAXWORKERS 16      /* maximum number of workers threads */
#define MINSIZE 1000       /* minimum array size, if we have minimum than this, using sequential quicksort */

int numWorkers;
int size; 
int arr[MAXSIZE];
double start_time, end_time;

int isSorted(int arr[], int size);
int partition(int arr[], int left, int right);
void quickSort(int arr[], int left, int right);

int main(int argc, char *argv[]) {
    int i;
    
    /* read command line args if any */
    size = (argc > 1) ? atoi(argv[1]) : MAXSIZE;
    numWorkers = (argc > 2) ? atoi(argv[2]) : MAXWORKERS;
    /* Safety Check */
    if (size > MAXSIZE) size = MAXSIZE;
    if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

    omp_set_num_threads(numWorkers);    // setting the number of openmp threads 

    /* Initialize array with random values */
    for (i = 0; i < size; i++) {
        arr[i] = rand() % 1000000;
    }

    start_time = omp_get_wtime();
    
    // parallel region
    #pragma omp parallel 
    {
        #pragma omp single   // only one thread starts the initial quickSort, the rest takes tasks
        {
            quickSort(arr, 0, size - 1);
        }
    }
    // Implicit Barrier
    
    end_time = omp_get_wtime();
    
    printf("Array size: %d\n", size);
    printf("Threads: %d\n", numWorkers);
    printf("Sorted: %s\n", isSorted(arr, size) ? "Yes" : "No");
    printf("Time: %g seconds\n", end_time - start_time);

    return 0;
}


int partition(int arr[], int left, int right){
    int pivot = arr[right];      // choosing last element as pivot
    int i = left;          // index for smaller elements 

    for(int j = left ; j < right; j++){
        if(arr[j] < pivot){
            int temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
            i++;
        }
    }
    // put pivot in its position
    int temp = arr[i];
    arr[i] = arr[right];
    arr[right] = temp;

    return i;      // return's pivot's position
}


void quickSort(int arr[], int left, int right) {
    if (left < right) {
        int pivotIndex = partition(arr, left, right);

        // Creating tasks for other threads, only right and left sorting in parallel
        if(right - left >= MINSIZE){
            #pragma omp task         // task left
            quickSort(arr, left, pivotIndex - 1);    // Sort left part
        
            #pragma omp task         // task right
            quickSort(arr, pivotIndex + 1, right);   // Sort right part
        
            #pragma omp taskwait   // Wait for both tasks to finish, for cleaner, more efficent if we add function after, gaurannted correctnes
        } else {
            quickSort(arr, left, pivotIndex - 1);
            quickSort(arr, pivotIndex + 1, right);
        } 
    }
}


/* helper function to chec arrays are sorted */
int isSorted(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) {
            return 0;  // Not sorted
        }
    }
    return 1;  // Sorted
}