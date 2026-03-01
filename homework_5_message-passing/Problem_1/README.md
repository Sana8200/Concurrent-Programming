## Problem 1: Distributed Pairing - Client-Server Application

A distributed MPI application where a teacher (server) pairs students (clients) for project work.
A teacher needs to pair students for a project. The teacher is the server (process 0) and students are clients (processes 1 to n). 
Students send requests to the teacher, and the teacher pairs them together.

### Algorithm implemented 

The teacher process waits for students to send pairing requests. 
When a request is received, the teacher checks if another student is also waiting to be paired. 
If two requests are available, the teacher pairs the students together and sends each student their partner’s ID.

If there is an odd number of students, the last student will not have a partner. 
In this case, the student is paired with themselves and works alone.

Students send their IDs to the teacher using MPI_Send(). 
Since the teacher does not know which student will send first (parallel execution is non-deterministic), MPI_Recv() is used with MPI_ANY_SOURCE to receive messages from any studentm, (accepts requests from any source).

After pairing, the teacher sends back the partner ID to each student.


```
Student --> sends request --> Teacher
Student <-- gets partner  <-- Teacher
```

### How to run

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

### MPI functions used 
- MPI_Init() - start MPI
- MPI_Comm_rank() - get my process ID
- MPI_Comm_size() - get total processes
- MPI_Send() - send message
- MPI_Recv() - receive message (waits until message arrives)
- MPI_Finalize() - end MPI


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

- **Distributed**: The program is distributed, processes don't share any memory and they only communicate by sending message to each other using MPI.
- **Non-deterministic**: Pairing order depends on which students' requests arrive first, sutdents arrive in random orders and teacher pairs whoever arrived first (FIFO pairing)
- **Handles odd numbers**: Last student works alone and pair with themselves if odd number of students