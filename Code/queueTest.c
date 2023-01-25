#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "CarPark.h"

int psize = 16;			// The car-park capacity.
int inval = 3;			// The number of in-valets.
int outval = 4;			// The number of out-valets.
int qsize = 8;			// The capacity of the arrivals queue.
double expnum = 0.05;	// The expected number of arrivals.

int main() {

    Car** park = malloc(psize * sizeof(Car*));
    pthread_mutex_t park_lock;
    pthread_mutex_init(&park_lock, NULL);

    // Initialize the car park and its components
    // Initialize the queue with a capacity of 3
    Qinit(3);
    printf("Queue initialized with capacity of %d\n", Qcapacity());

    G2DInit(park, psize, inval, outval, park_lock);
    show();
    sleep(1);


    // Create a car and enqueue it
    Car car1;
    CarInit(&car1);
    Qenqueue(&car1);
    printf("Car enqueued. Queue size: %d\n", Qsize());

    show();
    sleep(1);

    // Create another car and enqueue it
    Car car2;
    CarInit(&car2);
    Qenqueue(&car2);
    printf("Car enqueued. Queue size: %d\n", Qsize());

    show();
    sleep(2);

    // Peek at the head of the queue
    Car* car = Qpeek();
    printf("Car at the head of the queue: ID=%d, Arrival Time=%ld, Leave Time=%ld, Image Name=%s\n", car->cid, car->atm, car->ltm, car->pn);

    show();
    sleep(2);

    // Serve the head of the queue
    car = Qserve();
    printf("Car served. Queue size: %d\n", Qsize());

    show();
    sleep(2);

    // Create another car and enqueue it
    Car car3;
    CarInit(&car3);
    Qenqueue(&car3);
    printf("Car enqueued. Queue size: %d\n", Qsize());

    show();
    sleep(1);

    // Create another car and enqueue it
    Car car4;
    CarInit(&car4);
    Qenqueue(&car4);
    printf("Car enqueued. Queue size: %d\n", Qsize());

    show();
    sleep(1);

    // Check if the queue is full
    if (QisFull()) {
        printf("Queue is full\n");
    } else {
        printf("Queue is not full\n");
    }

    sleep(2);
    show();

    // Get an iterator for the queue
    int size;
    Car** iterator = Qiterator(&size);
    printf("Queue contents: \n");
    for (int i = 0; i < size; i++) {
        printf("ID=%d, Arrival Time=%ld, Leave Time=%ld, Image Name=%s\n", iterator[i]->cid, iterator[i]->atm, iterator[i]->ltm, iterator[i]->pn);
    }

    sleep(2);
    show();

    // Clear the queue
    Qclear();
    printf("Queue cleared. Queue size: %d\n", Qsize());

    sleep(2);
    show();

    // Check if the queue is empty
    if (QisEmpty()) {
        printf("Queue is empty\n");
    } else {
        printf("Queue is not empty\n");
    }

    // Wait for the user to close the window
    SDL_Event event;
    int i = 0;
    while (i != 5) {
        // float rd = (rand() % 50)/100.0;
        // sleep(rd);
        printf("HI!%d\n", i);
        show();
		sleep(1);
        i++;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            printf("CLODS");
            i = 3;
            break;
        }
    }

    printf("END!");


    /** Calculate and print statistics **/
    // PrintStatistics();

    // Cleanup and exit

    // Free memory and destroy the lock
    Qfree();
    finish();
    free(park);
    pthread_mutex_destroy(&park_lock);

    return 0;
}