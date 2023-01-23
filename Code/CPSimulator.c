#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
// #include "Queue.h"
// #include "Car.h"
#include "CarPark.h"

#define PSIZE_MIN   12			// Minimum capacity of the parking space
#define PSIZE       16			// Default capacity of the parking space
#define PSIZE_MAX   40			// Maximum capacity of the parking space
#define IN_VAL_MIN  1			// Minimum number of in valets
#define IN_VAL      3			// Default number of in valets
#define IN_VAL_MAX  6			// Maximum number of in valets
#define OUT_VAL_MIN 1			// Minimum number of out valets
#define OUT_VAL     2			// Default number of out valets
#define OUT_VAL_MAX 6			// Maximum number of out valets
#define QSIZE_MIN   3			// Minimum size of the waiting cars queue
#define QSIZE       8			// Default size of the waiting cars queue
#define QSIZE_MAX   8			// Maximum size of the waiting cars queue
#define EXPNUM_MIN  1E-2	    // Minimum expected number of in-comming cars
#define EXPNUM      5E-2	    // Default expected number of in-comming cars
#define EXPNUM_MAX  1.5E-1	    // Maximum expected number of in-comming cars

// variables
int psize;
int psize_arg;
int inval;
int inval_arg;
int outval;
int outval_arg;
int qsize;
int qsize_arg;
double expnum;
double expnum_arg;
// Car** car_park;
// Queue* queue;
pthread_mutex_t park_lock;

/* Statistical Variables
*
* 	During the simulation, these variables will be used 
* 	to collect data on the performance of the car-park activities.
*/
int oc = 0;		// Current number of occupied slots in the parking space.
int nc = 0;		// Running total number of cars created during the simulation
int pk = 0; 	// Running total number of cars allowed to park
int rf = 0; 	// Running total number of cars not allowed to park
int nm = 0; 	// The number of cars currently acquired by in-valets
long sqw= 0; 	// Running sum of car-waiting times in the arrival queue
long spt= 0; 	// Running sum of car-parking durations
double ut = 0; 	// Current car-park space utilization

/* Time Variables
* 
* 	These variables will be used to print the times in the simulator's final report.
*/
time_t current_t;			// The current time of each loop in the monitor thread
time_t start_t;				// Time of simulator start
time_t received_sgnl_t;		// Time of received shutdown signal
time_t shtdown_t;			// Time the car park is shut down
time_t val_leav_t;			// Time of leaving the valets
time_t val_lft_t;			// Time of left the valets
time_t monitor_exit_t;		// Time of left the monitor
time_t stop_t;				// Simulator end time

void* in_valet_func(void*);
void* out_valet_func(void*);
void* car_arrival_func(void*);

/* Signal handling */
static volatile int stop = 0;
/* An event handler to stop the program when pressing ctrl^c */ 
void handle(int sig){
	received_sgnl_t = time(NULL);
	stop = 1;
	shtdown_t = time(NULL);
}

int main(int argc, char* argv[]) {

    // initialize with default values
    psize = PSIZE;
    inval = IN_VAL;
    outval = OUT_VAL;
    qsize = QSIZE;
    expnum = EXPNUM;

    // Handle command line arguments
    if (argc == 1) { 
        ; // nop
    }
    if (argc < 1 || argc >= 7) {
        printf("Invalid Arguments");
        return 1;
    }
    if (argc >= 2) {
        psize_arg = atoi(argv[1]);
        if (!(psize_arg < PSIZE_MIN || psize_arg > PSIZE_MAX)) {
            psize = psize_arg;
        } else {
            printf("Invalid park size. Using default value %d\n", PSIZE);
        }
    }
    if (argc >= 3) {
        inval_arg = atoi(argv[2]);
        if (!(inval_arg < IN_VAL_MIN || inval_arg > IN_VAL_MAX)) {
            inval = inval_arg;
        } else {
            printf("Invalid number of in-valets. Using default value %d\n", IN_VAL);
        }
    }
    if (argc >= 4) {
        outval_arg = atoi(argv[3]);
        if (!(outval_arg < OUT_VAL_MIN || outval_arg > OUT_VAL_MAX)) {
            outval = outval_arg;
        } else {
            printf("Invalid number of out-valets. Using default value %d\n", OUT_VAL);
        }
    }
    if (argc >= 5) {
        qsize_arg = atoi(argv[4]);
        if (!(qsize_arg < QSIZE_MIN || qsize_arg > QSIZE_MAX)) {
            qsize = qsize_arg;
        } else {
            printf("Invalid queue size. Using default value %d\n", QSIZE);
        }
    }
    if (argc >= 6) {
        expnum_arg = atof(argv[5]);
        if (!(expnum_arg < EXPNUM_MIN || expnum_arg > EXPNUM_MAX)) {
            expnum = expnum_arg;
        } else {
            printf("Invalid expected number of cars. Using default value %lf\n", EXPNUM);
        }
    }

    printf("Car park size: %d\n", psize);
    printf("Number of in valets: %d\n", inval);
    printf("Number of out valets: %d\n", outval);
    printf("Queue size: %d\n", qsize);
    printf("Expected number of cars: %f\n", expnum);
    
    /* Init. signal handler to abort the program when wanted */
	// signal(SIGINT, handle);

    // Initialize the car park and its components
    Car** park = malloc(psize * sizeof(Car*));
    pthread_mutex_t park_lock;
    pthread_mutex_init(&park_lock, NULL);

    // Qinit(qsize);
    G2DInit(park, psize, inval, outval, park_lock);
    // show();

    // Create and start the valet threads
    // pthread_t in_valet_threads[inval];
    // for(int i = 0; i < inval; i++) {
    //     pthread_create(&in_valet_threads[i], NULL, inValet, (void*)i);
    // }
    // pthread_t out_valet_threads[outval];
    // for(int i = 0; i < outval; i++) {
    //     pthread_create(&out_valet_threads[i], NULL, outValet, (void*)i);
    // }

    // // Create the car generator thread
    // pthread_t car_gen;
    // pthread_create(&car_gen, NULL, carGenerator, (void*)EXP_CARS);

    // // Wait for all threads to finish
    // for(int i = 0; i < inval; i++) {
    //     pthread_join(in_valet_threads[i], NULL);
    // }
    // for(int i = 0; i < outval; i++) {
    //     pthread_join(out_valet_threads[i], NULL);
    // }
    // pthread_join(car_gen, NULL);

    // Wait for the user to close the window
    SDL_Event event;
    int i = 0;
    while (i != 6) {
        // float rd = (rand() % 50)/100.0;
        // sleep(rd);
        printf("HI!%d\n", i);
        show();
		sleep(1);
        i++;
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            printf("CLODS");
            i = 6;
            break;
        }
    }

    printf("HI!");


    /** Calculate and print statistics **/
    // Cleanup and exit

    // Free memory and destroy the lock
    Qfree();
    free(park);
    pthread_mutex_destroy(&park_lock);

    return 0;
}




