/*
 * Problem 2: Peer-to-Peer Pairing
 * 
 * Based on Lecture 16 (MPI) and Lecture 17 (Peer communication)
 * 
 * Process 0 = Teacher (only starts the algorithm)
 * Process 1,2,3... = Students (pair with each other directly)
 * 
 * Algorithm:
 * 1. Teacher randomly picks a student and sends "your turn" with list of all students
 * 2. Student with turn: picks random partner, sends message to partner
 * 3. Partner receives: now paired! Passes "your turn" to next random student
 * 4. Continue until all paired
 * 
 * Message count: n messages (optimal)
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define YOUR_TURN -1
#define TAG 0

void teacher(int num_students);
void student(int my_id, int num_students);

int main(int argc, char *argv[]) {
    int rank, size;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    int num_students = size - 1;
    
    if(num_students < 1){
        if(rank == 0){
            printf("Error: At least 2 processes are required (1 teacher + 1 student).\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Seed random differently for each process
    srand(time(NULL) + rank * 100);
    
    if (rank == 0) {
        teacher(num_students);
    } else {
        student(rank, num_students);
    }
    
    MPI_Finalize();
    return 0;
}

void teacher(int num_students) {
    /*
     * Teacher only starts the algorithm:
     * - Creates list of all students
     * - Picks random student to start
     * - Sends "your turn" message
     */
    
    // Message format: [action, count, student1, student2, ...]
    // action = YOUR_TURN (-1) or partner_id
    int msg_size = num_students + 2;
    int *msg = (int*)malloc(msg_size * sizeof(int));
    
    msg[0] = YOUR_TURN;
    msg[1] = num_students;
    
    // Fill list with student IDs (1 to n)
    for (int i = 0; i < num_students; i++) {
        msg[2 + i] = i + 1;
    }
    
    // Pick random student to start (from the list)
    int random_idx = rand() % num_students;
    int first = msg[2 + random_idx];
    
    printf("Teacher: Starting peer-to-peer pairing for %d students\n", num_students);
    printf("Teacher: Randomly picked Student %d to start\n\n", first);
    
    MPI_Send(msg, msg_size, MPI_INT, first, TAG, MPI_COMM_WORLD);
    
    free(msg);
}

void student(int my_id, int num_students) {
    /*
     * Student algorithm:
     * - Wait for message
     * - If YOUR_TURN: pick random partner, send to them
     * - If partner_id: I was picked! Pass turn to next random student
     */
    
    int msg_size = num_students + 2;
    int *msg = (int*)malloc(msg_size * sizeof(int));
    
    MPI_Recv(msg, msg_size, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    int action = msg[0];
    int count = msg[1];
    
    if (action == YOUR_TURN) {
        // It's my turn to pick a partner
        
        // 1. Remove myself from list
        for (int i = 0; i < count; i++) {
            if (msg[2 + i] == my_id) {
                msg[2 + i] = msg[2 + count - 1];  // swap with last
                break;
            }
        }
        count--;
        
        // 2. Check if anyone left to pair with
        if (count == 0) {
            // I'm the last one - work alone (odd number case)
            printf("Student %d: I am working alone (odd one out)\n", my_id);
        } else {
            // Pick random partner from remaining
            int random_idx = rand() % count;
            int partner = msg[2 + random_idx];
            
            // 3. Remove partner from list
            msg[2 + random_idx] = msg[2 + count - 1];  // swap with last
            count--;
            
            // Print my index and my partner's index
            printf("Student %d: My partner is Student %d\n", my_id, partner);
            
            // 4. Send to partner (tell them I'm their partner + remaining list)
            msg[0] = my_id;  // I am your partner
            msg[1] = count;
            MPI_Send(msg, msg_size, MPI_INT, partner, TAG, MPI_COMM_WORLD);
        }
        
    } else {
        // I was picked by someone
        int partner = action;
        
        // Print my index and my partner's index
        printf("Student %d: My partner is Student %d\n", my_id, partner);
        
        // If there are students left, pass "your turn" to random one
        if (count > 0) {
            int random_idx = rand() % count;
            int next = msg[2 + random_idx];
            
            msg[0] = YOUR_TURN;
            MPI_Send(msg, msg_size, MPI_INT, next, TAG, MPI_COMM_WORLD);
        }
    }
    
    free(msg);
}