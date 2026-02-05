# Homework 2: Parallel Programming with OpenMP

This repository contains solutions for the OpenMP parallel programming assignment, demonstrating shared-memory parallelism concepts using C and OpenMP.

## Overview

The assignment explores two fundamental parallel computing patterns:
- **Q1: Data Parallelism** - Parallel reduction operations on a matrix
- **Q2: Task Parallelism** - Recursive parallelism with OpenMP tasks

## Project Structure

```
homework-2-openmp/
├── README.md
├── Q1/
│   ├── README.md
│   ├── matrixSum-openmp.c
│   └── results/           # Benchmark results (if applicable)
└── Q2/
    ├── README.md
    ├── quickSort_openmp.c
    └── results/           # Benchmark results (if applicable)
```

## Problems

| Problem | Description | Points | Parallelization Strategy |
|---------|-------------|--------|--------------------------|
| Q1 | Matrix Sum, Min, Max | 20 | `parallel for` with reduction and critical sections |
| Q2 | Parallel Quicksort | 20 | Recursive task parallelism with `omp task` |

## Requirements

- GCC 4.4 or later with OpenMP support
- Linux/Unix environment (tested on Ubuntu)

## Quick Start

### Compile all programs:
```bash
# Q1: Matrix Sum
cd Q1
gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c

# Q2: Quicksort
cd ../Q2
gcc -O -fopenmp -o quickSort_openmp quickSort_openmp.c
```

### Run with custom parameters:
```bash
# Q1: ./matrixSum-openmp <matrix_size> <num_threads>
./matrixSum-openmp 5000 4

# Q2: ./quickSort_openmp <array_size> <num_threads>
./quickSort_openmp 1000000 4
```

## Key OpenMP Concepts Used

### Q1 - Matrix Operations
- `#pragma omp parallel for` - Distributes loop iterations across threads
- `reduction(+:total)` - Thread-safe summation
- `#pragma omp critical` - Mutual exclusion for min/max updates
- Double-checked locking pattern for performance optimization

### Q2 - Quicksort
- `#pragma omp parallel` - Creates a team of threads
- `#pragma omp single` - Single thread initiates recursion
- `#pragma omp task` - Creates tasks for recursive calls
- `#pragma omp taskwait` - Synchronization point
- Cutoff threshold (`MINSIZE`) to avoid task overhead for small arrays

## Performance Measurement

Both programs use `omp_get_wtime()` to measure only the parallel computation time, excluding initialization.

To control the number of threads:
```bash
# Method 1: Environment variable
export OMP_NUM_THREADS=4
./program

# Method 2: Command line argument (built into programs)
./program <size> <num_threads>
```

## Speedup Analysis

Speedup is calculated as:
```
Speedup = Sequential Time / Parallel Time
```

Factors affecting speedup:
- **Problem size** - Larger problems show better speedup (Amdahl's Law)
- **Thread overhead** - Creating/synchronizing threads has cost
- **Cache effects** - Memory access patterns impact performance
- **Load balancing** - Uneven work distribution reduces efficiency


Concurrent Programming - KTH Royal Institute of Technology
