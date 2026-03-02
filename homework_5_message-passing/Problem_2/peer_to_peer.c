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
    
    srand(time(NULL) + rank * 100);     // get different random for each process (teacher and students)
     
    if (rank == 0) {
        teacher(num_students);
    } else {
        student(rank, num_students);
    }
    
    MPI_Finalize();
    return 0;
}

// Teacher funtion to start the peer-to-peer pairing algorithm
void teacher(int num_students) {
    /* Teacher creates list of all students, picks random studnets to start
       and sendd "your turn" message to them. 
       After that, students will handle the rest of the pairing themselves.
    */
    printf("Teacher preparing to start peer-to-peer pairing for %d students...\n", num_students);
    // Message format: [action, count, student1, student2, ...]
    int msg_size = num_students + 2;
    int *msg = (int*)malloc(msg_size * sizeof(int));
    
    msg[0] = YOUR_TURN;
    msg[1] = num_students;
    
    // Fill list with student IDs (1 to n)
    for (int i = 0; i < num_students; i++) {
        msg[2 + i] = i + 1;
    }
    
    // Pick random student to start (from the list) - could be fixed to 1 for deterministic 
    int random_id = rand() % num_students;
    int first = msg[2 + random_id];
    
    printf("Teacher: Randomly picked Student %d to start pairing...\n\n", first);
    
    MPI_Send(msg, msg_size, MPI_INT, first, TAG, MPI_COMM_WORLD);
    
    free(msg);
}


// Student function to handle receiving messages and pairing logic
void student(int my_id, int num_students) {
    /* Students receive the message in the format: [action, count, student1, student2, ...]
       if action == YOUR_TURN: it's my turn to pick a partner
       if action == partner_id: I was picked by partner_id, I am their partner
    */
    
    int msg_size = num_students + 2;
    int *msg = (int*)malloc(msg_size * sizeof(int));
    
    MPI_Recv(msg, msg_size, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    int action = msg[0];
    int count = msg[1];
    
    if (action == YOUR_TURN) {
        // It's my turn to pick a partner
    
        //Remove myself from list
        for (int i = 0; i < count; i++) {
            if (msg[2 + i] == my_id) {
                msg[2 + i] = msg[2 + count - 1];  // swap with last
                break;
            }
        }
        count--;
        
        // Check if anyone left to pair with
        if (count == 0) {
            // I'm the last one - work alone (odd number case)
            printf("Student %d: I am working alone...\n", my_id);
        } else {
            // Pick random partner from remaining
            int random_id = rand() % count;
            int partner = msg[2 + random_id];
            
            // Remove partner from list
            msg[2 + random_id] = msg[2 + count - 1];  // swap with last
            count--;
            
            printf("Student %d: My partner is Student %d\n", my_id, partner);
            
            // Send to partner (tell them I'm their partner + remaining list)
            msg[0] = my_id;  // I am your partner
            msg[1] = count;
            MPI_Send(msg, msg_size, MPI_INT, partner, TAG, MPI_COMM_WORLD);
        }
        
    } else {
        // I was picked by someone
        int partner = action;
        
        printf("Student %d: My partner is Student %d\n", my_id, partner);
        
        // If there are students left, pass "your turn" to random one
        if (count > 0) {
            int random_id = rand() % count;
            int next = msg[2 + random_id];
            
            msg[0] = YOUR_TURN;
            MPI_Send(msg, msg_size, MPI_INT, next, TAG, MPI_COMM_WORLD);
        }
    }
    free(msg);
}