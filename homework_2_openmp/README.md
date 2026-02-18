## Homework 2: Programming with OpenMP

Shared-memory parallel programming using OpenMP.

### Problems Solved

| File | Description |
|------|-------------|
| `matrixSum-openmp.c` | Matrix sum/min/max using `parallel for` with reduction |
| `quickSort_openmp.c` | Parallel quicksort using `task` directives |

### Compilation

```bash
gcc -O -fopenmp -o program program.c
```

### Usage

```bash
./matrixSum-openmp 5000 4       # 5000×5000 matrix, 4 threads
./quickSort_openmp 1000000 4   # 1M elements, 4 threads
```

### Approaches

**Q1 (Matrix Sum/Min/Max):**  
- `reduction(+:total)` for thread-safe summation  
- `critical` sections with double-checked locking for min/max  
- Only parallel section is timed using `omp_get_wtime()`

**Q2 (Quicksort):**  
- `parallel` + `single` to initialize, other threads pick up tasks  
- `task` directives spawn work for left/right subarrays  
- Cutoff threshold (`MINSIZE=1000`) to avoid task overhead on small arrays  
- `taskwait` ensures correctness before returning
