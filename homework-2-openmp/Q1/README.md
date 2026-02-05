# Q1: Parallel Matrix Sum, Min, and Max

Compute the sum, minimum, and maximum elements of a matrix in parallel using OpenMP.

## Problem Description

Extend the basic matrix summation program to:
1. Compute the **sum** of all matrix elements
2. Find the **maximum** element and its position (row, column)
3. Find the **minimum** element and its position (row, column)

All operations are performed in parallel using OpenMP constructs.

## Implementation

### Algorithm

The program uses a `parallel for` directive to distribute matrix rows across threads:

```c
#pragma omp parallel for reduction(+:total) private(j)
for (i = 0; i < size; i++)
    for (j = 0; j < size; j++) {
        total += matrix[i][j];
        // Update max/min with critical sections
    }
```

### Key OpenMP Features

| Feature | Purpose |
|---------|---------|
| `parallel for` | Distributes outer loop iterations across threads |
| `reduction(+:total)` | Thread-safe summation with automatic combining |
| `private(j)` | Each thread gets its own copy of inner loop variable |
| `critical(find_max)` | Mutual exclusion when updating global maximum |
| `critical(find_min)` | Mutual exclusion when updating global minimum |

### Optimization: Double-Checked Locking

To reduce critical section overhead, a double-check pattern is used:

```c
if (max < matrix[i][j]) {           // First check (no lock)
    #pragma omp critical(find_max)
    {
        if (max < matrix[i][j]) {   // Second check (with lock)
            max = matrix[i][j];
            max_row = i;
            max_col = j;
        }
    }
}
```

This avoids entering the critical section unless there's a potential update.

## Compilation

```bash
gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
```

## Usage

```bash
./matrixSum-openmp <matrix_size> <num_workers>
```

**Parameters:**
- `matrix_size`: Size of the N×N matrix (default: 10000, max: 10000)
- `num_workers`: Number of OpenMP threads (default: 8, max: 8)

**Examples:**
```bash
# Default: 10000x10000 matrix with 8 threads
./matrixSum-openmp

# 5000x5000 matrix with 4 threads
./matrixSum-openmp 5000 4

# 1000x1000 matrix with 2 threads
./matrixSum-openmp 1000 2
```

## Sample Output

```
the total is 2449504851
it took 0.0523 seconds
the max is 98 at position [0, 42]
the min is 0 at position [0, 15]
```

## Benchmarking

To measure speedup, run with different thread counts:

```bash
# Sequential baseline (1 thread)
./matrixSum-openmp 5000 1

# Parallel runs
./matrixSum-openmp 5000 2
./matrixSum-openmp 5000 4
./matrixSum-openmp 5000 8
```

### Recommended Test Configurations

| Matrix Size | Description |
|-------------|-------------|
| 1000×1000 | Small - may show overhead effects |
| 5000×5000 | Medium - good for speedup analysis |
| 10000×10000 | Large - best parallel efficiency |

Run each configuration **at least 5 times** and use the **median** value.

## Expected Speedup Behavior

- **Small matrices**: Limited speedup due to thread creation overhead
- **Large matrices**: Better speedup as computation dominates overhead
- **Diminishing returns**: Beyond optimal thread count, cache contention increases

### Factors Affecting Performance

1. **Critical section contention** - Multiple threads may compete for max/min updates
2. **Cache locality** - Row-major access pattern is cache-friendly
3. **False sharing** - Avoided by having threads work on separate rows
4. **Reduction overhead** - Combining partial sums at the end

## Notes

- Matrix is initialized with random values (0-98) for testing
- Only the parallel computation time is measured (not initialization)
- Thread count is set via `omp_set_num_threads()` for portability