/* The linux tee command 
   This program uses producer consumer approach
   3 threads (1 producer, 2 consumers) - one shared buffer */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 10

char buffer[BUFFER_SIZE];
int dataReady = 0;      // 0 no data - 1 new data available 
int readersDone = 0;        // readersDone = 2, producer knows consumers finished
int done = 0;                // done = 1, producers set done when finished 

pthread_mutex_t lock;
pthread_mutex_t readersDoneLock;       // Readers signals consumers, needs lock
pthread_mutex_t dataReadLock;        

void *Producer(void *);
void *Consumer(void *);


int main(){
   //Ininitializing the mutex locks
   pthread_mutex_init(&lock, NULL);
   pthread_mutex_init(&readersDoneLock, NULL);
   pthread_mutex_init(&dataReadLock, NULL);
   

}

void *Producer(void *arg){
   while(1){

   }
}

void *Consumer(void *arg){
   while(1){
      
   }


}
