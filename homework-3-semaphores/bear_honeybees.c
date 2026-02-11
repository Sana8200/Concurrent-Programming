/* Bear and Honeybees Problem (multiple producers - one consumer) 

   gcc -o bear_honeybees bear_honeybees.c -lpthread
   ./bear_honeybees <capacity> <bees>
*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define MAXPORTIONS 20
#define MAXHONEYBEES 30    

sem_t pot;                 // Mutex for common pot access
sem_t full;                // signal: pot is full, wake bear
sem_t empty;               // signal: bear finished eating, pot is empty
sem_t turn[MAXHONEYBEES];  // Array for round-robin ordering

int honeyPortions;         // current honey portions in the pot
int H;                     // capacity of the pot
int numHoneyBees;

void *HoneyBees(void *arg);
void *Bear(void *arg);


int main(int argc, char *argv[]){
    //setbuf(stdout, NULL);     // disables output buffering, in case if we print without \n
    
    pthread_t honeyBees[MAXHONEYBEES];
    pthread_t bear;

    /* read command line args if any */
    H = (argc > 1)? atoi(argv[1]) : MAXPORTIONS;
    numHoneyBees = (argc > 2)? atoi(argv[2]): MAXHONEYBEES;
    /* safey checck */
    if(numHoneyBees > MAXHONEYBEES) numHoneyBees = MAXHONEYBEES;
    if( H > MAXPORTIONS) H = MAXPORTIONS;

    honeyPortions = 0;     // pot starts empty

    sem_init(&pot, 0, 1);      // mutex, init 1 (binary semaphore)
    sem_init(&full, 0, 0);     // signaling, init 0
    sem_init(&empty, 0, 0);    // signaling, init 0
    // Initialize turns: Bee 0 goes first, others wait
    for (int i = 0; i < numHoneyBees; i++) {
        sem_init(&turn[i], 0, (i == 0) ? 1 : 0);
    }


    // Creating threads (bear 1 thread, Honeybees N threads)
    pthread_create(&bear, NULL, Bear, NULL);    
    for(int i = 0; i < numHoneyBees; i ++){
        pthread_create(&honeyBees[i], NULL, HoneyBees, (void *)(long) i);
    }

    // Wait for threads(in this case threads can run forever, but we can have it anyway)
    for(int i = 0; i < numHoneyBees; i++){
        pthread_join(honeyBees[i], NULL);
    }

    return 0; 
}

/* Multiple Procuders (Honeybees)*/
void *HoneyBees(void *arg){
    int id = (int)(long) arg;
    int next = (id + 1) % numHoneyBees;    // Next honey bee in the circle
    while(1){
        sem_wait(&turn[id]);       // wait for my turn
        sem_wait(&pot);            // enter CS
        honeyPortions++;
        printf("Bee %d added honey to the pot. Portions: %d/%d\n", id, honeyPortions, H);
        if (honeyPortions == H) {
            printf("Bee %d filled the pot! Waking the bear!!!\n", id);
            sem_post(&full);       // wake up the bear
            sem_wait(&empty);      // wait for bear to finish eating
        }
        sem_post(&pot);            // leave CS
        sem_post(&turn[next]);     // signal next bee
        sleep(rand() % 3 + 1);
    }
}


/* One Consumer (the bear) */
void *Bear(void *arg){
    printf("Bear is sleeping, waiting for honey pot to be full (capacity: %d)\n", H);
    while (1) {
        sem_wait(&full);           // sleep until pot is full
        honeyPortions = 0;         // eat all the honey
        printf("Bear woke up and ate all the honey! Pot is now empty.\n");
        sem_post(&empty);          // signal bee that pot is empty
    }
}