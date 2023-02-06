#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

// The pthread structures

pthread_t *in_valet_threads_id;
pthread_t *out_valet_threads_id;
pthread_t monitor_thread_id;
pthread_t car_gen_thread_id;


sem_t empty;			// Counting semaphore for waiting while the parking is empty to avoid the busy-waiting.
// sem_t full;				// Counting semaphore for waiting while the parking is full to avoid the busy-waiting.
pthread_mutex_t park_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t arrival_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t departure_cond = PTHREAD_COND_INITIALIZER;
Car** parking_array;

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
* TODO TAMMAM
* 	These variables will be used to print the times in the simulator's final report.
*/ 
time_t current_t;			// The current time of each loop in the monitor thread
time_t start_t;				// Time of simulator start
time_t received_sgnl_t;		// Time of received shutdown signal
time_t shutdown_t;			// Time the car park is shut down
time_t val_leave_t;			// Time of leaving the valets
time_t val_left_t;			// Time of left the valets
time_t monitor_exit_t;		// Time of left the monitor
time_t stop_t;				// Simulator end time

void* out_valet_func(void* arg);
void* in_valet_func(void* arg);
void testCP();
int poissonRandom(double);

void* car_gen_func(void*);
void* monitor_func(void*);

/* Thread function for the monitor */
void* monitor_func(void* arg) {
    while (1) {
        // Wait for a random period before checking the status of the car park
        // sleep(rand() % 10 + 1);
        sleep(2);
        // Acquire the lock
        pthread_mutex_lock(&park_lock);
        // Print and display the status of the car park TAMMAM
        // printf("Spots: []\n");
        // Update the graphical display
        show();
        // Release the lock 
        pthread_mutex_unlock(&park_lock);
    }
}

void PrintStatistics() {

	printf("\nSimulator started at:\t\t%s", ctime(&start_t));
	printf("  Park Space Capacity was:\t%d\n", psize);
	printf("  Allowed queue length was:\t%d\n", qsize);
	printf("  Number of in valets was:\t%d\n", inval);
	printf("  Number of out valets was:\t%d\n", outval);
	printf("  Expected arrivals was:\t%.2f\n", expnum);
	printf("Simulator stopped at:\t\t%s", ctime(&stop_t));

	printf("\nCP Simulation was executed for:\t%ld seconds\n", stop_t - start_t);
	printf("Total number of cars processed:\t%d cars\n", nc+rf);
	printf("  Number of cars that parked:\t%d cars\n", pk);
	printf("  Number of cars turned away:\t%d cars\n", rf);
	printf("  Number of cars in transit:\t%d cars\n", nc-pk-Qsize());
	printf("  Number of cars still queued:\t%d cars\n", Qsize());
	// printf("  Number of cars still parked:\t%d cars\n\n", PQsize());
	
	printf("Average queue waiting time:\t\t%.2f seconds\n", (float) sqw/pk);
	// printf("Average parking time:\t\t%.2f seconds\n", (float) spt/(pk-PQsize()));
	printf("Percentage of park utilization:\t%.2f%%\n\n", ut);
}

void cancel_threads() {

    for(int i = 0; i< inval; i++){
		pthread_cancel(in_valet_threads_id[i]);
	}

    for(int i = 0; i < outval; i++){
		pthread_cancel(out_valet_threads_id[i]);
	}

    pthread_cancel(monitor_thread_id);
    pthread_cancel(car_gen_thread_id);
    printf("CANCELED\n");
}

/* Signal handler for KILL */
void sigterm_kill(int signo) {
    // cancel_threads();
    // finish();

    printf("YOU KILL ME!?");

    // Cleanup and exit
    // Qfree();
    // Free memory and destroy the lock
    // free(parking_array);
    // pthread_mutex_destroy(&park_lock);
    // pthread_cond_destroy(&arrival_cond);
    // pthread_cond_destroy(&departure_cond);

    exit(0);
}

/* Signal handler for QUIT */
void sigterm_quit(int signo) {
    // cancel_threads();
    // finish();

    printf("YOU QUITTER!");

    // Cleanup and exit
    // Qfree();
    // Free memory and destroy the lock
    // free(parking_array);
    // pthread_mutex_destroy(&park_lock);
    // pthread_cond_destroy(&arrival_cond);
    // pthread_cond_destroy(&departure_cond);

    exit(0);
}

/* Signal handler for Ctrl^C */
void sigterm_handler(int signo) {
    received_sgnl_t = time(NULL);
    stop_t = time(NULL);
	val_leave_t = time(NULL);
    // char* current_time = ctime(&received_sgnl_t);
    printf("\b\b%s:\t Received shutdown shut down signal..\n", strtok(ctime(&received_sgnl_t), "\n"));
    // printf("%s:\t Car park is shutting down..\n", current_time);
    printf("%s:\t The valets are leaving..\n", strtok(ctime(&val_leave_t), "\n"));
    cancel_threads();
    printf("%s:\t Done. %d valets left.\n", strtok(ctime(&val_leave_t), "\n"), inval+outval);
    // printf("%s:\t Monitor exiting ...\n", current_time);
    
    // Shutdown graphical system 
    finish();

    /** Calculate and print statistics **/
    PrintStatistics();

    // Cleanup and exit
    Qfree();
    // Free memory and destroy the lock
    free(parking_array);
    pthread_mutex_destroy(&park_lock);
    pthread_cond_destroy(&arrival_cond);
    pthread_cond_destroy(&departure_cond);
    sem_post(&empty);

    exit(0);
}

/* Thread function for in-valets */
void* in_valet_func(void* arg) {
    int id = *(int*)arg;
    while (1) {
        /* Wait for a car to arrive */ 
        // printf("in valet %d checks Q %d\n", id, QisEmpty());
        // printf("in valet %d size Q %d\n", id, Qsize());
        while (QisEmpty()) {
            sleep((rand() % 20)/100.0);	// get a random value between 0 and 0.2
            // printf("in valet %d in arrival COND\n", id);
            pthread_cond_wait(&arrival_cond, &park_lock);
            // printf("singaled valet %d in arrival COND\n", id);
        }
        // printf("in valet %d out arrival COND\n", id);
        Car* carToServe = Qserve();
        setViState(id, FETCH);
        float rd = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
		sleep(rd);
		setViCar(id, carToServe);
        carToServe->vid = id;
        printf("in valet %d fetched car\n", id);
        sleep((rand() % 200)/100.0);	// get a random value between 0 and 0.2

        // TODO: TAMMAM
        setViState(id, WAIT);
        while (QisEmpty) {
            float rd = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
			sleep(rd);
            // sem_wait(&empty);
            pthread_cond_wait(&departure_cond, &park_lock);
        }
        // TODO: TAMMAM
        pthread_mutex_lock(&park_lock);
        float rd_wt = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
		sleep(rd_wt);

        setViState(id, MOVE);
		nm++; // The number of cars currently acquired by in-valets
		pk++; // Running total number of cars allowed to park
		float rd_pk = (rand() % 100)/100.0; // get a random value between 0 and 1
		sleep(rd_pk);

        (*carToServe).ptm = current_t;	
        sqw = sqw + (carToServe->ptm - carToServe->atm);

        printf("in valet %d aquired lock\n", id);
        /* Park the car */
        /* TODO: TAMMAM */
        pthread_mutex_unlock(&park_lock);
        printf("in valet %d released lock\n", id);
        // carToServe->sno = the park slot from array TAMMMAM

        sleep((rand() % 20)/100.0);			// get a random value between 0 and 0.2
		setViState(id, READY);
		nm--;
        /* Pause for a random period before and after parking the car */
        sleep(rand() % 10000 + 1);
        /* Pause for a random period in the critical section */
        usleep(rand() % 200000 + 1);
    }
    free(arg);
    pthread_exit(0);
}

/* Thread function for out-valets */
void *out_valet_func(void* arg) {
    int id = *(int*)arg;
    while (1) {
        setVoState(id, WAIT);

        /* Wait for a park spot */
        // TAMMAM PARK SIZE
        // while (park is full) {
            // sleep((rand() % 20)/100.0)
            // pthread_cond_wait(&deppart_cond, &park_lock);
        // }

        sleep((rand() % 20)/100.0);

        pthread_mutex_lock(&park_lock);
        printf("out valet %d aquired lock\n", id);
        while (1 == 0) {
            pthread_cond_wait(&arrival_cond, &park_lock);
        }
        /* Park the car */
        // car_park.num_arrivals--;
        // car_park.num_available--;
        pthread_mutex_unlock(&park_lock);
        printf("out valet %d released lock\n", id);
        oc--;
        setVoState(id, READY);
        /* Pause for a random period before and after parking the car */
        sleep(rand() % 1000 + 1);
        /* Pause for a random period in the critical section */
        usleep(rand() % 200000 + 1);
    }
    free(arg);
    pthread_exit(0);
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
    
    // Initialize the car park and its components
    // int parkings_size;
    // parking_array = PQiterator(&parkings_size); // TODO TAMMAM
    parking_array = malloc(psize * sizeof(Car*));
    pthread_mutex_init(&park_lock, NULL);
    // pthread_cond_init(&arrival_cond, NULL);
    // pthread_cond_init(&departure_cond, NULL);  
    sem_init(&empty, 0, psize);

    Qinit(qsize);
    printf("Queue initialized with capacity of %d\n", Qcapacity());
    G2DInit(parking_array, psize, inval, outval, park_lock);
    show();
    sleep(1);

    // Set up the signal handler for SIGTERM to abort program
    signal(SIGINT, sigterm_handler);
    signal(SIGQUIT , sigterm_quit);
    signal(SIGKILL , sigterm_kill);

    // Get the default attributes for threads
	pthread_attr_t attr;
    pthread_attr_init(&attr);

    // Create the monitor thread
    pthread_t monitor_thread;
    if (pthread_create(&monitor_thread, &attr, monitor_func, NULL) != 0) {
        printf("Failed to Create Monitor Thread");
        return 1;
    }
    monitor_thread_id = monitor_thread;

    // Create and start the valet threads
    pthread_t in_valet_threads[inval];
    for(int i = 0; i < inval; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&in_valet_threads[i], &attr, in_valet_func, id) != 0) {
            printf("Failed to Create In Valet Thread #%d", *id);
            return 1;
        }
    }
    in_valet_threads_id = in_valet_threads;

    pthread_t out_valet_threads[outval];
    for(int i = 0; i < outval; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&out_valet_threads[i], &attr, out_valet_func, id) != 0) {
            printf("Failed to Create Out Valet Thread #%d", *id);
            return 1;
        }
    }
    out_valet_threads_id = out_valet_threads;

    // Create the car generator thread
    pthread_t car_gen_thread;
    if (pthread_create(&car_gen_thread, &attr, car_gen_func, NULL) !=0) {
        printf("Failed to Create Car Generator Thread");
        return 1;
    }
    car_gen_thread_id = car_gen_thread;

    if (pthread_join(car_gen_thread, NULL) != 0) {
        printf("Finished Simulation");
        return 2;
    }
	// stop_t = time(NULL);
	// val_leav_t = time(NULL);
    // while (1) {

        // sleep(5);
        // printf("HHHH\n");
        // Generate an incoming car with a Poisson distribution 
        // double arrival_time = -logf(1.0f - (float)rand() / RAND_MAX) / arrival_rate;
        // sleep((int)arrival_time);
        // Add the car to the arrival queue 
        // pthread_mutex_lock(&park_lock);
        // if (car_park.num_arrivals < num_spots) {
        // car_park.num_arrivals++;
        // pthread_cond_signal(&arrival_cond);
        // } else {
        // / Refuse service and send the car away */
        // }
        // pthread_mutex_unlock(&park_lock);
    // }


    // printf("CRASH!");
    // return 0;
    // testCP();
}

void* car_gen_func(void* arg) {
    /* Start the simulation */ 
	start_t = time(NULL);

	// /* Main Thread
	// * 	Entering an endless loop until receiving the signal.
	// *	
	// *	It will constantly create cars and enter them into the 
	// *	arrival queue if it is not full.
	// */
    // // Enter an endless loop where it generates incoming cars 
	while(1) {
        // testCP();
		int num_cars = newCars((double)1);		
		// int num_cars = 5;		
        printf("num_cars: %d\n", num_cars);// get a random number with Poisson distribution
        show();
		for(int i = 0; i < num_cars; i++) {
            // printf("%d", i);
			Car *car = (Car*) malloc(sizeof(Car));	// Allocate memory for new car	// Allocate memory for new car
			// pthread_mutex_lock(&Qlock);  // Lock the arrival queue
			if (!QisFull()){
				CarInit(car);	// init. a car 
				nc++;			// this car is allowed to park
                Qenqueue(car);	// the car enqueued
                pthread_cond_signal(&arrival_cond);
                printf("SIGNALLED\n");
            } else {
                pthread_cond_signal(&arrival_cond);
				rf++;			// this car is not allowed to park
			}
			float rd = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
			updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
			sleep(rd);
		    show();
		}
        // show();
		sleep(1);
	}/* End of simulation*/
    // exit(0);
}

void testCP() {
    // /* TEST SCRIPT FOR QUEUE
	// * 	TAMMAM MAYBE YOU CAN TRY TO TEST YOUR PARK STRUCTURE HERE
	// */
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
    sleep(1);

    // Peek at the head of the queue
    Car* car = Qpeek();
    printf("Car at the head of the queue: ID=%d, Arrival Time=%ld, Leave Time=%ld, Image Name=%s\n", car->cid, car->atm, car->ltm, car->pn);

    show();
    sleep(1);

    // Serve the head of the queue
    car = Qserve();
    printf("Car served. Queue size: %d\n", Qsize());

    show();
    sleep(1);

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

    sleep(1);
    show();

    // Get an iterator for the queue
    int size;
    Car** iterator = Qiterator(&size);
    printf("Queue contents: \n");
    for (int i = 0; i < size; i++) {
        printf("ID=%d, Arrival Time=%ld, Leave Time=%ld, Image Name=%s\n", iterator[i]->cid, iterator[i]->atm, iterator[i]->ltm, iterator[i]->pn);
    }

    sleep(1);
    show();

    // Clear the queue
    Qclear();
    printf("Queue cleared. Queue size: %d\n", Qsize());

    sleep(1);
    show();

    // Check if the queue is empty
    if (QisEmpty()) {
        printf("Queue is empty\n");
    } else {
        printf("Queue is not empty\n");
    }

    // Wait for the user to close the window
    // SDL_Event event;
    // int i = 0;
    // while (i != 5) {
    //     // float rd = (rand() % 50)/100.0;
    //     // sleep(rd);
    //     printf("HI!%d\n", i);
    //     show();
	// 	sleep(1);
    //     i++;
    //     if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
    //         printf("CLODS");
    //         i = 3;
    //         break;
    //     }
    // }

    // printf("END!");


    /** Calculate and print statistics **/
    // PrintStatistics();

    // Cleanup and exit

    // Free memory and destroy the lock
    // Qfree();
    // finish();
    // free(parking_array);
    // pthread_mutex_destroy(&park_lock);

    // exit(0);
}


int poissonRandom(double expectedValue) {
    printf("ddddd%f", expectedValue);
    int n = 0; //counter of iteration
    double limit; 
    double x;  //pseudo random number
    limit = exp(-expectedValue);
    x = rand(); 
    while (x > limit) {
    n++;
    x *= rand();
    }
    return n;
}