/* The linux tee command 
   This program uses producer consumer approach
   3 threads (1 producer, 2 consumers) - one shared buffer
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>       

#define BUFFER_SIZE 1024

char buffer[BUFFER_SIZE];
int readersDone[2] = {1, 1};   // array to keep track of eachh consumer if finished reading, 1 = has read, 0 = needs to read
int done = 0;    // done = 1, producer finished (EOF) 
char *filename;  

pthread_mutex_t lock;         // one lock for everything 
pthread_cond_t dataAvailable;    // cv - producer signals consumers
pthread_cond_t readDone;         // cv - consumers signal producer

void *Producer(void *arg);     // Producer which will read from stdin (standard input)
void *Consumer(void *arg);     // Consumer which will write to stdout AND to a file from the shared buffer 

int main(int argc, char *argv[]){
    // first we need to check the arguments passed
    if(argc != 2){
        printf("Usage: tee filename\n");
        return 1;
    }
    
    filename = argv[1];  // Storeing filename globally

    //Ininitializing  mutex locks and condition variables 
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&dataAvailable, NULL);
    pthread_cond_init(&readDone, NULL);

    long id0 = 0, id1 = 1; 

    // Creating the thread (producer, attribute, function, argument)
    pthread_t producer, consumer0, consumer1;
    pthread_create(&producer, NULL, Producer, NULL);
    pthread_create(&consumer0, NULL, Consumer, (void *) id0);
    pthread_create(&consumer1, NULL, Consumer, (void *) id1);

    // WAiting for all thread
    pthread_join(producer, NULL);
    pthread_join(consumer0, NULL);
    pthread_join(consumer1, NULL);

    // cleaning up after finsihed 
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&dataAvailable);
    pthread_cond_destroy(&readDone);

    return 0;
}


/* Prodcuer 
   Reads a line from stdin into temp buffer
   Waits for both consumers to finish reading previous data
   Copies to shared buffer and signals consumers
   Repeats until EOF, On EOF, sets done = 1 and wakes consumers so they can exit
*/
void *Producer(void *arg){
    char tempBuffer[BUFFER_SIZE];   // temporary local buffer for producer to avoid race condition

    // wait for both consumers to finish reading the previous data
    while(fgets(tempBuffer, BUFFER_SIZE, stdin) != NULL){
        pthread_mutex_lock(&lock);
        while(!(readersDone[0] && readersDone[1])){
            pthread_cond_wait(&readDone, &lock);
        }
        // copy the data read from input to shared buffer
        strcpy(buffer, tempBuffer);

        // Reset flags - both consumers need to read new data
        readersDone[0] = 0;
        readersDone[1] = 0;

        pthread_cond_broadcast(&dataAvailable);   // wake up both threads 
        pthread_mutex_unlock(&lock);
    }

    // EOF - wait for consumers to finish last data (they need to be finished with previous data), then signal done -
    pthread_mutex_lock(&lock);
    while(!(readersDone[0] && readersDone[1])){
        pthread_cond_wait(&readDone, &lock);
    }
    done = 1;
    pthread_cond_broadcast(&dataAvailable);
    pthread_mutex_unlock(&lock);

    return NULL;
}

/* Consumer Function - writes to stdout and to a file (two threads with same function)
   Uses local buffer - writing to stdout and file is slow, fast copy, then release the lock for producer
   and also producer might overwrite shared buffer before we finish printing
*/
void *Consumer(void *arg){
    long myId = (long) arg;    
    char localBuffer[BUFFER_SIZE];
    
    FILE *fptr = NULL;
    if(myId == 1){
        fptr = fopen(filename, "w");
        if(fptr == NULL){
            printf("couldn't open the file\n");
            return NULL;
        }
    }

    while(1){
        pthread_mutex_lock(&lock);

        //waiting for new data (the consumers flag readersDone[myId]) ( redersDone = 0, done = 0)
        while(readersDone[myId] && !done){
            pthread_cond_wait(&dataAvailable, &lock);
        }

        // exit condition - when done = 1 adn consumer already read the lat data
        if(readersDone[myId] && done){
            pthread_mutex_unlock(&lock);
            break;
        }

        // copy data from shared buffer to local buffer 
        strcpy(localBuffer, buffer);
        readersDone[myId] = 1;     // Consumer finished reading, marking its flag as 1

        // if both consumers done, signal producer
        if(readersDone[0] && readersDone[1]){
            pthread_cond_signal(&readDone);
        }
        pthread_mutex_unlock(&lock);

        if(myId == 0){
            printf("%s", localBuffer);      // writing to standard output outsiide the lock
        } else if(myId == 1){
            fprintf(fptr, "%s", localBuffer);   // writing to file outside the lock
        }
    }

    if(fptr != NULL){     
        fclose(fptr);
    }
    return NULL;
}