## Problem 2: Distributed Pairing - Peer-To-Peer Application

Students pair with each other directly without the teacher managing the pairing. The teacher only starts the algorithm by randomly picking one student and telling them "your turn to pick a partner."
- Process 0 = Teacher (only starts the algorithm)
- Process 1,2,3... = Students (pair with each other directly)

### Algorithm implemented 
1. Teacher randomly picks a student and sends them a message "your turn" with the list of all students
2. That student picks a random partner from the list and sends them a message
3. The partner receives the message (now they're paired) and passes "your turn" to another random student
4. This continues until everyone is paired
5. If odd number of students, the last one works alone


The message contains: `[action, count, student1, student2, ...]`
- `action = -1` means "your turn to pick"
- `action = partner_id` means "I picked you as my partner"
- `count` = how many students are still unpaired
- The list contains the remaining unpaired students

Students send messages directly to each other using MPI_Send(). This is peer-to-peer - the teacher is not involved after starting.

The solution uses excatly n messages:
- 1 message from teacher to first student
- Then each pairing takes 1 message (picker tells partner)
- Plus 1 message to pass "your turn" to next student

For example for 4 students: 4 messages total.

### How to run
Compile:
```
mpicc -o peer_to_peer peer_to_peer.c
```

Run:
```
mpirun -np 5 ./peer_to_peer    # 4 students
mpirun -np 7 ./peer_to_peer    # 6 students

mpirun --oversubscribe -np 7 ./peer_to_peer   # if not enough slots
```


### Example Output (6 students)
```
Teacher: Starting peer-to-peer pairing for 6 students
Teacher: Randomly picked Student 3 to start

Student 3: My partner is Student 4
Student 4: My partner is Student 3
Student 1: My partner is Student 6
Student 6: My partner is Student 1
Student 2: My partner is Student 5
Student 5: My partner is Student 2
```

### Example Output (5 students - odd)
```
Teacher: Starting peer-to-peer pairing for 5 students
Teacher: Randomly picked Student 2 to start

Student 2: My partner is Student 3
Student 3: My partner is Student 2
Student 4: My partner is Student 5
Student 5: My partner is Student 4
Student 1: I am working alone...
```
- **Distributed**: The application is distributed because processes don't share any memory. Students communicate directly with each other by sending messages using MPI. The teacher only initiates - students decide their partners and pairing themselves. 
- **Non-deterministic**: Different pairs each run because of random selection
- **Peer-to-peer**: Students talk directly to students, not through teacher