# Q2: Parallel Quicksort with OpenMP Tasks

A parallel implementation of the Quicksort algorithm using OpenMP task-based parallelism with recursive decomposition.

## Problem Description

Implement a parallel quicksort that:
1. Divides the array into sublists using a pivot
2. Recursively sorts sublists **in parallel** using OpenMP tasks
3. Demonstrates speedup with multiple threads

## Implementation

### Algorithm Overview

```
quicksort(arr, left, right):
    if left < right:
        pivot_index = partition(arr, left, right)
        
        if (right - left) >= MINSIZE:
            // Create parallel tasks for large subarrays
            task: quicksort(arr, left, pivot_index - 1)
            task: quicksort(arr, pivot_index + 1, right)
            taskwait
        else:
            // Sequential recursion for small subarrays
            quicksort(arr, left, pivot_index - 1)
            quicksort(arr, pivot_index + 1, right)
```

### Key OpenMP Features

| Feature | Purpose |
|---------|---------|
| `#pragma omp parallel` | Creates a team of threads |
| `#pragma omp single` | Only one thread starts the initial recursion |
| `#pragma omp task` | Creates a task for each recursive call |
| `#pragma omp taskwait` | Waits for child tasks to complete |

### Parallel Region Structure

```c
#pragma omp parallel 
{
    #pragma omp single   // One thread initiates
    {
        quickSort(arr, 0, size - 1);
    }
}
// Implicit barrier - all threads wait here
```

### Cutoff Optimization

To avoid excessive task creation overhead, sequential recursion is used for small subarrays:

```c
#define MINSIZE 1000  // Threshold for task creation

if (right - left >= MINSIZE) {
    #pragma omp task
    quickSort(arr, left, pivotIndex - 1);
    
    #pragma omp task
    quickSort(arr, pivotIndex + 1, right);
    
    #pragma omp taskwait
} else {
    // Sequential recursion
    quickSort(arr, left, pivotIndex - 1);
    quickSort(arr, pivotIndex + 1, right);
}
```

The `MINSIZE` threshold (1000 elements) balances:
- **Too small**: Excessive task overhead
- **Too large**: Insufficient parallelism

## Compilation

```bash
gcc -O -fopenmp -o quickSort_openmp quickSort_openmp.c
```

## Usage

```bash
./quickSort_openmp <array_size> <num_workers>
```

**Parameters:**
- `array_size`: Number of elements to sort (default: 10,000,000, max: 10,000,000)
- `num_workers`: Number of OpenMP threads (default: 16, max: 16)

**Examples:**
```bash
# Default: 10 million elements with 16 threads
./quickSort_openmp

# 1 million elements with 4 threads
./quickSort_openmp 1000000 4

# 100,000 elements with 2 threads
./quickSort_openmp 100000 2
```

## Sample Output

```
Array size: 1000000
Threads: 4
Sorted: Yes
Time: 0.0891 seconds
```

The program automatically verifies correctness by checking if the array is sorted.

## Benchmarking

### Recommended Test Configurations

| Array Size | Description |
|------------|-------------|
| 100,000 | Small - task overhead may dominate |
| 1,000,000 | Medium - good balance |
| 10,000,000 | Large - best parallelization potential |

### Running Speedup Tests

```bash
# Sequential baseline
./quickSort_openmp 1000000 1

# Parallel runs
./quickSort_openmp 1000000 2
./quickSort_openmp 1000000 4
./quickSort_openmp 1000000 8
```

Run each configuration **at least 5 times** and use the **median** value.

## Expected Speedup Behavior

### Why Quicksort Parallelizes Well

1. **Independent subproblems**: After partitioning, left and right subarrays are independent
2. **Recursive structure**: Natural fit for task-based parallelism
3. **Good granularity**: Tasks become smaller as recursion deepens

### Limiting Factors

1. **Sequential partitioning**: The partition step is inherently sequential
2. **Unbalanced splits**: Poor pivot selection leads to uneven task sizes
3. **Task overhead**: Creating/scheduling many small tasks has cost
4. **Memory bandwidth**: Large arrays may be memory-bound

### Typical Speedup Pattern

```
Threads:   1    2    4    8
Speedup:  1.0  1.7  2.8  3.5
```

Speedup is typically **sub-linear** due to:
- Amdahl's Law (sequential partition phase)
- Task scheduling overhead
- Cache contention

## Implementation Details

### Partition Strategy

Uses the last element as pivot (Lomuto partition scheme):

```c
int partition(int arr[], int left, int right) {
    int pivot = arr[right];
    int i = left;
    
    for (int j = left; j < right; j++) {
        if (arr[j] < pivot) {
            swap(arr[i], arr[j]);
            i++;
        }
    }
    swap(arr[i], arr[right]);
    return i;
}
```

### Correctness Verification

```c
int isSorted(int arr[], int size) {
    for (int i = 0; i < size - 1; i++) {
        if (arr[i] > arr[i + 1]) return 0;
    }
    return 1;
}
```

## Potential Improvements

1. **Better pivot selection**: Median-of-three to avoid worst-case partitions
2. **Hybrid approach**: Switch to insertion sort for very small subarrays
3. **Task cutoff tuning**: Adjust `MINSIZE` based on system characteristics
4. **Parallel partition**: Use parallel prefix sums for the partition step

## Notes

- Array is initialized with random values (0 to 999,999)
- Only the sorting time is measured (not initialization or verification)
- The `taskwait` ensures correctness by waiting for child tasks
- Global array avoids allocation overhead but limits flexibility