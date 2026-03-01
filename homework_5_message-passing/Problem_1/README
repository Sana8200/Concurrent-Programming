# Problem 1: Distributed Pairing - Client-Server Application

A distributed MPI application where a teacher (server) pairs students (clients) for project work.
A teacher needs to pair students for a project. The teacher is the server (process 0) and students are clients (processes 1 to n). 
Students send requests to the teacher, and the teacher pairs them together.

## How the algorithm works

1. Teacher waits for students to send pairing requests
2. When teacher gets two requests, it pairs those students together
3. Teacher tells each student who their partner is
4. If there's an odd number of students, the last one works alone

## Message passing

Students send their ID to the teacher using MPI_Send(). The teacher receives these with MPI_Recv() using MPI_ANY_SOURCE because we don't know which student will send first.

After pairing, teacher sends back the partner's ID to each student.

```
Student --> sends request --> Teacher
Student <-- gets partner  <-- Teacher
```

## How to run

Compile:
```
mpicc -o clientServer clientServer.c
```

Run (need n+1 processes for n students):
```
mpirun -np 5 ./clientServer    # 4 students
mpirun -np 4 ./clientServer    # 3 students, one works alone

mpirun --oversubscribe -np 7 ./clientServer   # for not enough slots error 
```
### It's distributed since..
The process don't share any memory. They only communicate by sending message to each other using MPI. 

## MPI functions used 
- MPI_Init() - start MPI
- MPI_Comm_rank() - get my process ID
- MPI_Comm_size() - get total processes
- MPI_Send() - send message
- MPI_Recv() - receive message (waits until message arrives)
- MPI_Finalize() - end MPI

### Why MPI_ANY_SOURCE?
The teacher doesn't know which student will send first (parallel execution is non-deterministic), so it accepts requests from any source.


## Example Output (4 students)
```
Student 2: Sending pairing request to teacher
Student 3: Sending pairing request to teacher
Teacher process is running to pair 3 students with each other...
Teacher: Received request from Student 3
Teacher: Received request from Student 2
Student 1: Sending pairing request to teacher
Teacher: Pairing Student 3 with Student 2
Teacher: Received request from Student 1
Teacher: Student 1 will work alone (odd number)
Teacher: All students have been paired!
Student 3: My partner is Student 2
Student 2: My partner is Student 3
Student 1: I am working alone (no partner)
```

- **Distributed**: Yes - processes communicate only via message passing, no shared memory
- **Non-deterministic**: Pairing order depends on which students' requests arrive first
- **Handles odd numbers**: Last student works alone if odd number of students