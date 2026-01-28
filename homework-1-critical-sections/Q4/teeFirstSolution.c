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
int redersDone[2] = {1, 1}; // array to keep track of eachh consumer if finished reading, 1 = has read, 0 = needs to read
int done = 0;                // done = 1, producer finished (EOF) 

pthread_mutex_t lock;       // mutex lock - one lock for everything 
pthread_cond_t dataAvailable;       // cv - producer signals consumers
pthread_cond_t readDone;            // cv - consumers signal producer

void *Producer(void *arg);         // Producer which will read from stdin (standard input)
void *ConsumerOutput(void *arg);   // Consumer 0 which will write to stdout from shared buffer
void *ConsumerFile(void *arg);     // Consumer 1 which will write to a file from the shared buffer


int main(int argc, char *argv[]){
   // first we need to check the arguments passed
   if(argc !=2){
      printf("command to use: tee filename\n");
      return 1;
   }

   //Ininitializing  mutex locks and condition variables 
   pthread_mutex_init(&lock, NULL);
   pthread_cond_init(&dataAvailable, NULL);
   pthread_cond_init(&readDone, NULL);

   // Creating the thread (producer, attribute, function, argument)
   pthread_t producer, consumerOut, consumerFile;
   pthread_create(&producer, NULL, Producer, NULL);
   pthread_create(&consumerOut, NULL, ConsumerOutput, NULL);
   pthread_create(&consumerFile, NULL, ConsumerFile, (void *)argv[1]);

   // WAiting for all thread
   pthread_join(producer, NULL);
   pthread_join(consumerOut, NULL);
   pthread_join(consumerFile, NULL);

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
   char readerBuffer[BUFFER_SIZE];  // temporary local buffer for producer to avoid race condition

   while(fgets(readerBuffer, BUFFER_SIZE, stdin) != NULL){    
      pthread_mutex_lock(&lock);

      // wait for both consumers to finish reading the previous data
      while(!(redersDone[0] && redersDone[1])){     
         pthread_cond_wait(&readDone, &lock);
      }

      // copy the data read from input to shared buffer  
      strcpy(buffer, readerBuffer);    

      // Reset flags - both consumers need to read new data
      redersDone[0] = 0;
      redersDone[1] = 0;

      pthread_cond_broadcast(&dataAvailable);   // wake up both consumers
      pthread_mutex_unlock(&lock);
   }

   // EOF - wait for consumers to finish last data (they need to be finished with previous data), then signal done -
   pthread_mutex_lock(&lock);
   while(!(redersDone[0] && redersDone[1])){    
      pthread_cond_wait(&readDone, &lock);
   }
   done = 1;
   pthread_cond_broadcast(&dataAvailable);  
   pthread_mutex_unlock(&lock);

   return NULL;
}


/* Consumer 0 - writes to stdout
   Uses local buffer because writing to stdout is slow,
   and producer might overwrite shared buffer before we finish printing
*/
void *ConsumerOutput(void *arg){
   char localBufferOutput[BUFFER_SIZE];

   while(1){
      pthread_mutex_lock(&lock);

      //waiting for new data (the consumers flag readersDone[0]) ( redersDone = 0, done = 0)
      while(redersDone[0] && !done){   
         pthread_cond_wait(&dataAvailable, &lock);    
      }
      // exit condition - when done = 1 adn consumer already read the lat data
      if(redersDone[0] && done){      
         pthread_mutex_unlock(&lock);
         break;
      }

      // copy data from shared buffer to local buffer 
      strcpy(localBufferOutput, buffer);     
      redersDone[0] = 1;      

      // if both consumers done, signal producer
      if(redersDone[0] && redersDone[1]){
         pthread_cond_signal(&readDone);
      }
      pthread_mutex_unlock(&lock);
      printf("%s", localBufferOutput);   // writing to standard output outsiide the lock
   }
   return NULL;
}


/* Consumer 1 - writes to file */
void *ConsumerFile(void *arg){
   char localBufferFile[BUFFER_SIZE];

   char *filename = (char *)arg;      //  get the filename from argument 
   FILE *fptr = fopen(filename, "w");
   if(fptr == NULL){
      printf("couldn't open the file");
      return NULL;    
   }
  
   while(1){
      pthread_mutex_lock(&lock);

      while(redersDone[1] && !done){
         pthread_cond_wait(&dataAvailable, &lock);
      }
      if(redersDone[1] && done){
         pthread_mutex_unlock(&lock);
         break;
      }

      strcpy(localBufferFile, buffer);
      redersDone[1] = 1;        // Consumer finished reading, marking its flag as 1

      // if both consumers done, signal producer
      if(redersDone[0] && redersDone[1]){
         pthread_cond_signal(&readDone);
      }
      pthread_mutex_unlock(&lock);
      fprintf(fptr, "%s", localBufferFile);
   }
   fclose(fptr);
   return NULL;
}