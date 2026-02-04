#include <stdlib.h>       
#include <stdio.h>


void quickSort(int *arr, int left, int right){
    if(left < right){
        int pivotIndex = partition(arr, left, right);
        quickSort(arr, left, pivotIndex - 1);    // Sort left part
        quickSort(arr, pivotIndex + 1, right);   // Sort right part
    }
}


// parallel quick sort: sorting left and right can run in parallel
