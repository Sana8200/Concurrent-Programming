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
int dataReady = 0;      // 0 no data - 1 new data available 
// readers finished counts the number of consumers which are done, starts at 2, because in the first iteration, the producer will wait forever if it's 0
int readersFinished = 2;    
int done = 0;                // done = 1, producer finished (EOF) 

pthread_mutex_t lock;       // mutex lock - one lock for everything 
pthread_cond_t dataAvailable;       // cd - producer signals consumers
pthread_cond_t readDone;       // cd - consumers signal producer

void *Producer(void *arg);     // Producer which will read from stdin (standard input)
void *ConsumerOutput(void *arg);  // Consumer 1 which will write to stdout (statndard output) from shared buffer
void *ConsumerFile(void *arg);  // Consumer 2 which will write to a file from the shared buffer 



int main(int argc, char *argv[]){
   // first we need to check the arguments passed
   if(argc !=2){
      printf("command to use: tee filename");
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


/* Prodcuer thread reads a line
   Waits for both consumers to finiish whatever in the buffer
   sets signals and signals consumers, dataReady = 1, readersFinished = 0
   Repeats until end of file (EOF)
   when reached EOF, sets done = 1, and wake consumers so they can exit
*/
void *Producer(void *arg){
   while(fgets(buffer, BUFFER_SIZE, stdin) != NULL){    // fgets reads a line from stdin
      pthread_mutex_lock(&lock);
      // waiting for both consumers to finish reading the prevous data( when finsihed -> readersFinished = 2)
      while(readersFinished < 2){     
         pthread_cond_wait(&readDone, &lock);    // so if readers are not done, the producer waits on cd all reads done and the mutex lock
      } // here after while both consumers are done with prevous data, so producer moves to processing new data and signaling consumers 
      readersFinished= 0;      // resets to 0, when readers finished once
      dataReady = 1;      // Signals that new data is available 

      pthread_cond_broadcast(&dataAvailable);      // tells all consumers that data is reads (wake them up)

      pthread_mutex_unlock(&lock);
   }

   // EOF - end of file, then producer needs to stop and signal consumers to exit, it's done
   pthread_mutex_lock(&lock);
   while(readersFinished < 2){    // waiting for the consumers to completely finished 
      pthread_cond_wait(&readDone, &lock);
   }

   done = 1;
   pthread_cond_broadcast(&dataAvailable);   // signal consumers
   pthread_mutex_unlock(&lock);

   return NULL;
}


/* Consumer writes to Stdout
   Needs to write to local buffer, since when writing to standard output directly
   producer overwrites buffer immediately, so consumer might not even finihed*/
void *ConsumerOutput(void *arg){
 // first we need local buffer that consumer can copy the shared buffer data into it
   char localBufferOutput[BUFFER_SIZE];

   while(1){
      pthread_mutex_lock(&lock);

      //waiting for new data from producer
      while(!dataReady && !done){     // while data Read = 0, done = 0 
         pthread_cond_wait(&dataAvailable, &lock);    
      }
      if(done && !dataReady){       // it's for when done = 1, so we reached EOF
         pthread_mutex_unlock(&lock);
         break;
      }

      // Now Consumer 2 starts processing, copying data into local buffer 
      strcpy(localBufferOutput, buffer);     
      
      readersFinished++;      // finished taking the data, now increment the counter

      // both consumers done, signaling producer (using signal becasue it's just one thread waiting), and reset data Ready
      if(readersFinished == 2){
         dataReady = 0;
         pthread_cond_signal(&readDone);
      }
      pthread_mutex_unlock(&lock);

      printf("%s", localBufferOutput);   // writing to standard output 
   }
   return NULL;
}


void *ConsumerFile(void *arg){
   char localBufferFile[BUFFER_SIZE];
   
   char *filename = (char *)arg;      //  get the filename from argument 
   FILE *fptr;

   fptr = fopen(filename, "w");
   if(fptr == NULL){
      printf("couldn't open the file");
      return NULL;    
   }

   while(1){
      pthread_mutex_lock(&lock);

      while(!dataReady && !done){
         pthread_cond_wait(&dataAvailable, &lock);
      }
      if(done && !dataReady){
         pthread_mutex_unlock(&lock);
         break;
      }

      strcpy(localBufferFile, buffer);
      readersFinished++;

      if(readersFinished == 2){
         dataReady = 0;
         pthread_cond_signal(&readDone);
      }
      pthread_mutex_unlock(&lock);
      fprintf(fptr, "%s", localBufferFile);

   }
   fclose(fptr);
   return NULL;
}
