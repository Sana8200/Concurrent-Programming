## Homework 1: Critical Sections, Locks, Barriers, and Condition Variables

Concurrent programming fundamentals using POSIX threads (pthreads).

### Problems Solved

| File | Description |
|------|-------------|
| `matrixSum_a.c` | Matrix sum/min/max using **barrier** — Worker[0] computes final results |
| `matrixSum_b.c` | Matrix sum/min/max using **mutex locks** — main thread prints results |
| `matrixSum_c.c` | Matrix sum/min/max using **bag of tasks** — dynamic row distribution |
| `pi.c` | Compute π using **adaptive quadrature** with parallel intervals |
| `tee.c` | Linux `tee` command — **producer-consumer** with 3 threads |

### Compilation

```bash
gcc -o program program.c -lpthread -lm
```

### Usage

```bash
./matrixSum_a 1000 4      # 1000×1000 matrix, 4 workers
./pi 4 0.0000001          # 4 workers, epsilon tolerance
echo "hello" | ./tee out.txt
```

### Approaches

**Q1a (Barrier):** Each worker computes partial sum/min/max for its strip. After barrier, Worker[0] combines results.

**Q1b (Mutex):** Workers update shared globals directly using separate locks for sum, min, max. Main uses `pthread_join` to wait.

**Q1c (Bag of Tasks):** Shared row counter with mutex. Workers fetch rows dynamically — better load balancing.

**Q3 (Pi):** Divide [0,1] interval among workers. Each computes area under f(x)=√(1-x²) using adaptive quadrature. Multiply by 4 for π.

**Q4 (Tee):** Producer reads stdin → shared buffer. Two consumers write to stdout and file. Uses condition variables for synchronization.
