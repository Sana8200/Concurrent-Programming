# Homework 1: Critical Sections, Locks, Barriers, and Condition Variables

This assignment focuses on understanding and implementing fundamental concepts in concurrent programming using **POSIX threads (pthreads)**.

## Learning Objectives

The goals of this assignment are to:

- Understand **critical section problems** and why mutual exclusion is required
- Safely access **shared variables** using synchronization mechanisms
- Implement **mutual exclusion** with locks (mutexes)
- Program **condition synchronization** using condition variables
- Build a **concurrent parallel program** with multiple threads sharing data
- Create, manage, and join threads
- Pass input parameters to threads and collect results
- Synchronize threads using **locks, barriers, and condition variables**
- Compile and run a **multithreaded program** using the pthreads library

This assignment provides hands-on experience with designing, implementing, and debugging concurrent programs in C.




problems 
bjectives
In this homework, you learn how

to solve the critical section problem, i.e., to access shared variables with mutual exclusion;
to program condition synchronization (waiting for conditions) using condition variables;
to build a concurrent (parallel) program of multiple threads with shared variables, namely
to create and join concurrent threads and pass input parameters and results;
to synchronize threads with locks, barriers, and condition variables,
to compile and run a concurrent program using Pthreads (or Java threads and concurrent utilities)
Reading/Links
Lecture 6: Introduction to Pthreads.
Lecture 7: Threads, Locks, and Conditions in Java SE JDK
Tutorial: POSIX Threads ProgrammingLinks to an external site.
Threads API Manual
Pthread APIs - IBMLinks to an external site.
Chapter 4 - Shared-memory programming with Pthreads in "An Introduction to Parallel Programming" by Peter S. Pacheco and Matthew Malensek. Available online via KTH library.
Programming environments (language and libraries):
C/C++, the Pthread library.
Optional: Java using threads and concurrent utilities, namely the ReentrantLockLinks to an external site. class and the Condition Links to an external site.interface from the java.util.concurrent.locksLinks to an external site. package
See also "Programming Environments. Related Links".
Task
There are 7 problems below, which are worth as follows.

Problems	Points
1-5	20 each
6, 7	40 each
You are to solve any combination of the problems that add up to 40 points. Read also general requirements for homework, grading, and bonus policies. You may discuss the meaning of questions with classmates, but the work you turn in must be yours alone.

Problems
1. Compute the Sum, Min, and Max of Matrix Elements (20 points)
The purpose of this problem is to introduce you to the Pthreads library. Download, compile, and run a program matrixSum.c Download matrixSum.c. The program computes a sum of matrix elements in parallel using Pthreads. Develop the following three modified versions of the program.
(a) Extend the program so that in addition to the sum, it finds and prints a value and a position (indexes) of the maximum element of the matrix and a value and a position of the minimum element of the matrix. To check your solution, initialize matrix elements to random values (rather than all ones like in the above example).
(b) Change the program developed in (a) so that the main thread prints the final results. Do not call the Barrier function, and do not use arrays for partial results, such as sums in the above example.
(c) Change the program developed in (b) so that it uses a "bag of tasks" that is represented as a row counter, which is initialized to 0. A worker gets a task (i.e., the number of the row to process) out of the bag by reading a value of the counter and incrementing the counter as described for the matrix multiplication problem in Slides 27-29 about the "bag of tasks" in Lecture 5.

2. Quicksort (20 points)
The quicksort algorithm sorts the list of numbers by first dividing the list into two sublists so that all the numbers if one sublist are smaller than all the numbers in the other sublist. This is done by selecting one number (called a pivot) against which all other numbers are compared: the numbers which are less than the pivot are placed in one sublist, and the numbers which more than the pivot are placed in another sublist. The pivot can be either placed in one sublist or withheld and placed in its final position. Develop a parallel multithreaded program (in C using Pthreads or in Java) with recursive parallelism that implements the quicksort algorithm for sorting an array of n values. Performance evaluation: Measure and print also the execution time of your program using the timesLinks to an external site. function or the gettimeofdayLinks to an external site. function (see how it is done in matrixSum.c Download matrixSum.c.). To calculate the execution time, read the clock after initializing all variables and just before creating the threads. Read the clock again after the computation is complete and the worker threads have terminated.

3. Compute pi  (20 points)
The points on a unit circle centered at the origin are defined by the function f(x) = sqrt(1-x2). Recall that the area of a circle is pi*r2, where r is the radius. The adaptive quadrature routine described in Lecture 1 can approximate the pi value, computing the area of the upper-right quadrant of a unit circle and then multiplying the result by 4. Develop a multithreaded program (in C using Pthreads or in Java) to compute pi for a given epsilon using a given number of processes (threads) np which is assumed to be a command-line argument (i.e., it's given). Performance evaluation: Measure and print also the execution time of your program using the timesLinks to an external site. function or the gettimeofdayLinks to an external site. function (see how it is done in matrixSum.c Download matrixSum.c.). To calculate the execution time, read the clock after initializing all variables and just before creating the threads. Read the clock again after the computation is complete and the worker threads have terminated.
