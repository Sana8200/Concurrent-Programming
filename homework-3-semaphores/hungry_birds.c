#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define MAXWORMS 20
#define MAXBABYBIRDS 30      /* maximum number of baby birds(threads) */

sem_t dish;
sem_t empty, full;
sem_t turn[MAXBABYBIRDS];

int attr;
int worms;
int W;       // number of worms in the dish
int numbBabyBirds;

void *ParentBird(void *arg);
void *BabyBirds(void *arg);


int main(int argc, char *argv[]){
    setbuf(stdout, NULL);     // disables output buffering
    
    pthread_t babyBirds[MAXBABYBIRDS];
    pthread_t parentBird;

    /* read command line args if any */
    W = (argc > 1)? atoi(argv[1]) : MAXWORMS;
    numbBabyBirds = (argc > 2)? atoi(argv[2]): MAXBABYBIRDS;
    /* safey checck */
    if(numbBabyBirds > MAXBABYBIRDS) numbBabyBirds = MAXBABYBIRDS;
    if( W > MAXWORMS) W = MAXWORMS;

    worms = W;     // dish starts full

    sem_init(&dish, 0, 1);     // mutex, init 1 (binary semaphore)
    sem_init(&empty, 0, 0);    // signaling, init 0 (signaling, coordinator semaphore)
    sem_init(&full, 0, 0);     // signaling, init 0 
    for (int i = 0; i < numbBabyBirds; i++) {
        sem_init(&turn[i], 0, (i == 0) ? 1 : 0);
    }

    // Creating threads (parentbird 1 thread, baby birds N threads)
    pthread_create(&parentBird, NULL, ParentBird, NULL);      
    for(int i = 0; i < numbBabyBirds; i ++){
        pthread_create(&babyBirds[i], NULL, BabyBirds, (void *)(long) i);
    }

    // Wait for threads(in this case threads can run forever, but we can have it anyway)
    for(int i = 0; i < numbBabyBirds; i++){
        pthread_join(babyBirds[i], NULL);
    }

    return 0; 
}


void *ParentBird(void *arg){
    printf("Parent Bird provided %d worms in the common dish for %d baby birds.\n", W, numbBabyBirds);
    while(1){
        sem_wait(&empty);
        worms = W;    // fill the dish 
        printf("filled the dish with fresh %d worms!!!\n", W);
        sem_post(&full);
    }
}


// Multiple Consumers (Baby Birds)
void *BabyBirds(void *arg){
    int id = (int)(long)arg;

    printf(" %d Baby Birds are starting to eat and sleep\n", numbBabyBirds);
    while(1){
        sem_wait(&turn[id]);      // wait for my turn 
        sem_wait(&dish);          // enter CS
        if(worms == 0){
            printf("Baby bird %d found the dish empty! Chirping for parent.\n", id);
            sem_post(&empty);     // wake up the parent bird
            sem_wait(&full);      // wait for the dish to be full by parent (producer)
        }
        worms--;
        printf("Baby bird %d ate a worm. Worms left: %d\n", id, worms);
        sem_post(&dish);          // leave critical section 
        sem_post(&turn[(id + 1) % numbBabyBirds]);    // signal next bird
        sleep(rand() % 3 + 1);    // sleep for random time, +1 so birds are always sleep for at least 1 second
    } 
}