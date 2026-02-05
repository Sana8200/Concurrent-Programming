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



Objectives
In this homework, you learn how

to write, build and execute a parallel program in OpenMP to show performance speedup on more than one processor;
to find dependencies in loops and to write the code so that it can correctly be executed in parallel.
Reading/Links
Lecture 9: Introduction to OpenMP
Chapter 5, "Shared-memory programming with OpenMP" in An Introduction to Parallel Programming, by Peter S. Pacheco and Matthew Malensek. (The book is available online at the KTH library).
Tutorial: OpenMPLinks to an external site.
openmp.orgLinks to an external site. (the site provides OpenMP documentation, compilers, tools, and other resources)
Tutorials&ArticlesLinks to an external site.
SpecificationsLinks to an external site.
OpenMP Compilers&ToolsLinks to an external site.
GCC's OpenMP ManualLinks to an external site.
Videos from IntelLinks to an external site.
Programming environments (language and libraries):
C/C++ with OpenMP
gcc version 4.4 or later. Compile an OpenMP program with the option "-fopenmp"
See also "Programming Environments. Related Links".
Task
There are 4 problems which are worth as follows:

Problems	Points
1-2	20 each
3-4	40 each
You are to solve any combination of the problems that add up to 40 points. Read also general requirements for homework, the marking, and bonus policies. You may discuss the meaning of questions with classmates, but the work you turn in must be yours alone.

Problems
1. Compute the Sum, Min, and Max of Matrix Elements (20 points)
The purpose of this problem is to introduce you to basic OpenMP usage:
Download, compile, and run a program matrixSum-openmp.c Download matrixSum-openmp.c. The program computes a sum of matrix elements in parallel using OpenMP. Develop and evaluate the following modified version of the program.
Extend the program so that in addition to the sum, it finds and prints a value and a position (indexes) of the maximum element of the matrix and a value and a position of the minimum element of the matrix. To check your solution, initialize elements of the matrix to random values. Use OpenMP constructs. Run the program on different numbers of processors and report the speedup (sequential execution time divided by parallel execution time) for different numbers of processors (up to at least 4) and different sizes of matrices (at least 3 different sizes). Run each program several (at least 5) times and use the median value for execution time. Try to provide reasonable explanations for your results. Measure only the parallel part of your program. Specify the number of processors used by specifying a different number of threads (set the OMP_NUM_THREADS environment variable or use a call to omp_set_num_threads(), see the OpenMP specification). To measure the execution time, use the omp_get_wtime function (see omp_get_wtimeLinks to an external site.).

2. Quicksort (20 points)
The quicksort algorithm sorts the list of numbers by first dividing the list into two sublists so that all the numbers if one sublist are smaller than all the numbers in the other sublist. This is done by selecting one number (called a pivot) against which all other numbers are compared: the numbers which are less than the pivot are placed in one sublist, and the numbers which more than the pivot are placed in another sublist. The pivot can be either placed in one sublist or withheld and placed in its final position.

Develop a parallel multithreaded program (in C/C++ using OpenMP tasks) with recursive parallelism that implements the quicksort algorithm for sorting an array of n values.

Run the program on different numbers of processors and report the speedup (sequential execution time divided by parallel execution time) for different numbers of processors (up to at least 4) and different workloads (at least 3 different lists of various sizes). Run each program several (at least 5)  times and use the median value for execution time. Try to provide reasonable explanations for your results. Measure only the parallel part of your program. Specify the number of processors used by specifying a different number of threads (set the OMP_NUM_THREADS environment variable or use a call to omp_set_num_threads(), see the OpenMP specification). To measure the execution time, use the omp_get_wtime function (see omp_get_wtimeLinks to an external site.).