#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define REQUEST_TAG 1
#define RESPONSE_TAG 2

void server(int num_students);
void client(int my_id);

int main(int argc, char *argv[]){
    int rank, size;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);    // get the rank of the process
    MPI_Comm_size(MPI_COMM_WORLD, &size);    // get the total number of processes

    int num_students = size - 1;   // process 0 is the server, the rest are clients (students)

    if(num_students < 1){
        if(rank == 0){
            printf("Error: At least 2 processes are required (1 teacher(server) + 1 student(client)).\n");
        }
        MPI_Finalize();
        return 1;
    }

    if(rank == 0){
        server(num_students);      // TEACHER(SERVER) PROCESS
    }else{
        client(rank);              // STUDENT (CLIENT) PROCESS     
    }
    MPI_Finalize();
    return 0;
}

// Teacher (server) logic to handle requests from students (clients) for pairing them together 
void server(int num_students){
    MPI_Status status;     // declare status for receiving messages
    int student1, student2;
    int req_recived = 0;

    printf("Teacher process is running to pair %d students with each other...\n", num_students);

    while(req_recived < num_students){
        MPI_Recv(&student1, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST_TAG, MPI_COMM_WORLD, &status);
        req_recived++;
        printf("Teacher: Received request from Student %d\n", student1);

        // checking if there's another student to pair with
        if(req_recived < num_students){
            // Receive second student's request
            MPI_Recv(&student2, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST_TAG, MPI_COMM_WORLD, &status);
            req_recived++;
            printf("Teacher: Received request from Student %d\n", student2);

            // Pairing 
            printf("Teacher: Pairing Student %d with Student %d\n", student1, student2);

            // Send info to both students who they are paired with
            MPI_Send(&student2, 1, MPI_INT, student1, RESPONSE_TAG, MPI_COMM_WORLD);
            MPI_Send(&student1, 1, MPI_INT, student2, RESPONSE_TAG, MPI_COMM_WORLD);
        } else {
            // for odd number of students, last student works alone, pairs with themselves
            printf("Teacher: Student %d will work alone (odd number)\n", student1);
            MPI_Send(&student1, 1, MPI_INT, student1, RESPONSE_TAG, MPI_COMM_WORLD);
        }
    }
    printf("Teacher: All students have been paired!\n");
}


void client(int my_id){
    MPI_Status status;  // declare status for receiving messages
    int partner;        

    // Send pairing request to teacher (process 0)
    printf("Student %d: Sending pairing request to teacher\n", my_id);
    MPI_Send(&my_id, 1, MPI_INT, 0, REQUEST_TAG, MPI_COMM_WORLD);
        
    // Wait for partner info from teacher
    MPI_Recv(&partner, 1, MPI_INT, 0, RESPONSE_TAG, MPI_COMM_WORLD, &status);
        
    if (partner == my_id) {
        printf("Student %d: I am working alone (no partner)\n", my_id);
    } else {
        printf("Student %d: My partner is Student %d\n", my_id, partner);
    }
}