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

// Define constant values
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
#define EXPNUM_MAX  15E-1	    // Maximum expected number of in-comming cars


// Variables
int psize;          //capacity of the parking space
int psize_arg;
int inval;          //number of in valets
int inval_arg;
int outval;         //number of out valets
int outval_arg;
int qsize;          //ize of the waiting cars queue
int qsize_arg;
double expnum;      //expected number of in-comming cars
double expnum_arg;


// The pthread structures
pthread_t *in_valet_threads_id;     // Thread for in-valet
pthread_t *out_valet_threads_id;    // Thread for out-valet
pthread_t monitor_thread_id;        // Monitor thread
pthread_t car_gen_thread_id;        // Car generation thread

pthread_mutex_t queue_lock;         // Queue Mutex
pthread_mutex_t park_lock;          // Park Mutex

// The Semaphore structures
sem_t P_emptySlots;			        // Counting semaphore for read/write if the parking is empty to avoid the busy-waiting.
sem_t P_haveCars;			        // Counting semaphore for read/write if the parking is notfull to avoid the busy-waiting.

pthread_cond_t outVcond;
sem_t Q_emptyQueue;                 // Counting semaphore for read/write if the queue is empty to avoid the busy-waiting.
sem_t Q_haveCars;                   // Counting semaphore for read/write if the queue is notfull to avoid the busy-waiting.

// Park Array Structure
Car** car_park_array;

/* Statistical Variables
 *
 * During the simulation, these variables will be used to gather 
 * and display some data about the performance of the car-park operations,.
 */
int oc = 0;		// Current number of occupied slots in the parking space.
int nc = 0;		// Running total number of cars created during the simulation
int pk = 0; 	// Running total number of cars allowed to park
int rf = 0; 	// Running total number of cars not allowed to park
int nm = 0; 	// The number of cars currently acquired by in-valets
long sqw= 0; 	// Running sum of car-waiting times in the arrival queue
long spt= 0; 	// Running sum of car-parking durations
double ut = 0; 	// Current car-park space utilization
double tot_ut = 0; 	// Total car-park space utilization
int times_monitored = 0;

/* Time Variables
 *
 * 	These variables will be used to print the times in the simulator's final report.
 */ 
time_t current_t;			
time_t start_t;				
time_t stop_t;				
time_t received_signal_t;	
time_t exit_t;		

// Threads functions
void* out_valet_func(void* arg);
void* in_valet_func(void* arg);
void* car_gen_func(void*);
void* monitor_func(void*);

// Helper functions
void PrintStatistics();
void cancel_threads();
void sigterm_quit(int signo);
void sigterm_handler(int signo);



/* print statistics helper function */
void PrintStatistics() {
	printf("\n  Simulator started at:\t\t%s", ctime(&start_t));
	printf("  Park Space Capacity was:\t%d\n", psize);
	printf("  Allowed queue length was:\t%d\n", qsize);
	printf("  Number of in valets was:\t%d\n", inval);
	printf("  Number of out valets was:\t%d\n", outval);
	printf("  Expected arrivals was:\t%.2f\n", expnum);
	printf("  Simulator stopped at:\t\t%s", ctime(&stop_t));

	printf("\n  CP Simulation was executed for:\t%ld seconds\n", stop_t - start_t);
	printf("  Total number of cars processed:\t%d cars\n", nc+rf);
	printf("  Number of cars that parked:\t\t%d cars\n", pk);
	printf("  Number of cars turned away:\t\t%d cars\n", rf);
	printf("  Number of cars in transit:\t\t%d cars\n", nc-pk-Qsize());
	printf("  Number of cars still queued:\t\t%d cars\n", Qsize());
	printf("  Number of cars still parked:\t\t%d cars\n\n", oc);
	
	printf("  Average queue waiting time:\t\t\t%.2f seconds\n", (float) sqw/pk);
	printf("  Average parking time:\t\t\t\t%.2f seconds\n", (float) spt/pk);
	printf("  Percentage of current park utilization:\t%.2f%%\n", ut);
	printf("  Percentage of average park utilization:\t%.2f%%\n\n", tot_ut/(float)times_monitored);
}

/* cancel thread helper function */
void cancel_threads() {

    for(int i = 0; i< inval; i++){
		pthread_cancel(in_valet_threads_id[i]);
	}

    for(int i = 0; i < outval; i++){
		pthread_cancel(out_valet_threads_id[i]);
	}

    pthread_cancel(monitor_thread_id);
    pthread_cancel(car_gen_thread_id);
    // printf("THREADS CANCELED\n");
} 

/* Signal handler for QUIT */
void sigterm_quit(int signo) {
    printf("YOU QUIT!\n");
    exit(0);
}

/* Signal handler for Ctrl^C */
void sigterm_handler(int signo) {
    received_signal_t = time(NULL);
    printf("\b\b%s:\t Received shutdown shut down signal..\n", strtok(ctime(&received_signal_t), "\n"));
    printf("%s:\t Car park is shutting down..\n", strtok(ctime(&received_signal_t), "\n"));
    printf("%s:\t The valets are leaving...\n", strtok(ctime(&received_signal_t), "\n"));
    stop_t = time(NULL);
    cancel_threads();
    exit_t = time(NULL);
    printf("%s:\t Done. %d valets left.\n", strtok(ctime(&exit_t), "\n"), inval+outval);
    printf("%s:\t Monitor exiting ...\n", strtok(ctime(&exit_t), "\n"));
    
    //Calculate and print statistics
    PrintStatistics();
    
    // Free Data structures memory
    Qfree();
    // pQfree();
    Afree();

    // Destroy the mutexes & semaphores
    pthread_mutex_destroy(&park_lock);
    pthread_mutex_destroy(&queue_lock);
    sem_destroy(&Q_emptyQueue);
    sem_destroy(&P_emptySlots);
    sem_destroy(&P_haveCars);
    sem_destroy(&Q_haveCars);
    pthread_cond_destroy(&outVcond);
    // exit
    printf("%s:\t CarPark exits.\n", strtok(ctime(&exit_t), "\n"));
    exit(0);
}

/* Thread function for in-valets */
void* in_valet_func(void* arg) {
    int id = *(int*)arg;
    while (1) {

        show();
        sleep(0.01);
        sem_wait(&Q_haveCars);              // if Queue have cars continue else wait here

        pthread_mutex_lock(&queue_lock);    //lock the arrival Queue
        //================ Enter CS section for Queue ===================
        setViState(id, FETCH);              // set the valet state to "FETCH"
        Car* carToServe = Qserve();         // pop a car from the queue to the valte
        sleep((rand() % 20)/100.0);         // get a random value between 0 and 0.2
        //================ Exit CS for Queue ==================================
        pthread_mutex_unlock(&queue_lock);  // unlock the queue so new cars can come (exit secton for Queue)
        sleep((rand() % 100)/100.0);        // get a random value between 0 and 1

        setViCar(id, carToServe);
        carToServe->vid = id;
        nm++;                               // The number of cars currently acquired by in-valets

        setViState(id, WAIT);               // waiting to gain access the park 
        show();
        sleep(0.01);
        sem_wait(&P_emptySlots);            // wait if no empty slots in the parking, wait here all valet will wait here
        
        pthread_mutex_lock(&park_lock);     //aquire the lock all valet will compet here to take the lock
        //======================= CS for Park ==================================
        show();
        sleep(0.01);
        setViState(id, MOVE);               //busy (parking the car)
        sleep((rand() % 20)/100.0);         // get a random value between 0 and 0.2
        
        carToServe->sno = Aenqueue(carToServe);
        carToServe->ptm = time(NULL);       //like (*carToServe).ptm = current_t 
        carToServe->ltm = time(NULL) + rand() % 180;

        oc++;   // Current number of occupied slots in the parking space.
        pk++;   // increment number of cars that parked through simulation
        sqw += difftime(carToServe->ptm, carToServe->atm); // sum time from arriving untill parking (time in arrival queue for all cars)

        // pQenqueue(carToServe);              // park the car  copy (pointer only) for out valet  
        sem_post(&P_haveCars);
        //================== Exit section for Park ====================================
        pthread_mutex_unlock(&park_lock);
		
        nm--; 
		show();
        sleep(0.01);
        setViState(id, READY);              // set the valet state to "READY"
        updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
        sleep((rand() % 100)/100.0);        // get a random value between 0 and 1
    }
    free(arg);
    pthread_exit(0);
}

/* Thread function for out-valets */
void *out_valet_func(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&P_haveCars);              //wait if no car in the parking, wait here decrease number of cars in parking
        pthread_mutex_lock(&park_lock);
        pthread_cond_wait(&outVcond, &park_lock);
        setVoState(id, WAIT);               //waiting to access the park 
        show();
        sleep(0.01);

        //==================== Enter CS for Park ============================   
        Car* checkedCar = Apeek();
        setVoState(id, MOVE);

        Car* carToMove = Aserve(checkedCar->sno); // remove the car from the park
        setVoCar(id, carToMove);    // set the car acquired by the out-valet
        double stayed = difftime(time(NULL), carToMove->ptm);
        spt += stayed;              // sum time from parking untill exiting
        oc--;                       // decrement number of occupied slots in the parking space.
        sem_post(&P_emptySlots);    // one car left not full Increase empty places
        show();
        sleep(0.01);
        //================== Exit CS for Park ====================================
        pthread_mutex_unlock(&park_lock);
        updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);  
        setVoState(id, READY); //waiting to access the park 
        show();
        sleep((rand() % 100)/100.0);        // get a random value between 0 and 1
    }
    free(arg);
    pthread_exit(0);
}

/* Thread function for generating cars */
void* car_gen_func(void* arg) {
    /* Start the simulation */ 
	start_t = time(NULL);
    double prob = *(double*)arg;
    // Enter an endless loop where it generates incoming cars 
	while(1) {
		int num_cars = newCars(prob);	//generate pseudo random	
    	pthread_mutex_lock(&queue_lock);  // Lock the arrival queue
		//===== Enter CS for Queue ====
        for(int i = 0; i < num_cars; i++) {
			Car *car = (Car*) malloc(sizeof(Car));	// Allocate memory for new car	// Allocate memory for new car
			if (!QisFull()){    // this car is allowed to park
				nc++;			// incerement cars created
				CarInit(car); 
                Qenqueue(car);	// enqueue the car
                sem_post(&Q_haveCars); // post that Q have cars to allow im valet to start their work
            } else {    // this car is not allowed to park
				rf++;   // incerement refused cars
			}
            // wait before adding the next car
			updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
		    show();
            sleep((rand() % 20)/100.0);        // get a random value between 0 and 0.2
		}
        //===== Exit CS for Queue ====
        pthread_mutex_unlock(&queue_lock);
        // wait before generating the next car
		sleep((rand() % 100)/100.0);        // get a random value between 0 and 0.2
        
	}/* End of simulation*/
    free(arg);
    pthread_exit(0);
}

/* Thread function for the monitor */
void* monitor_func(void*) {
	Car **car_park;
	time_t prev_t = time(NULL);
	while(1){
		current_t = time(NULL);
		pthread_mutex_lock(&park_lock);			// Lock the arrival queue
		// === Enter CS for park ====
        car_park = Aiterator(&psize);	        // get an array of acrs in the parking
		int duration = current_t - prev_t;		// calculate the time from the last iteration
		/* Print and update the state of the parking */
		printf("Monitor: Number of cars in carpark: %d\n", Asize());
		printf("Slot:\t|");
		for (int i = 0; i < psize; i++){
			printf("%d\t|",i+1);
		}
		printf("\n\t|");
		for (int i = 0; i < psize; i++){
			printf("\t|");
		}
		printf("\nPark:\t|");
		for (int i = 0; i < psize; i++) {
			if (car_park[i]) {
				printf("%d\t|",car_park[i]->cid);
			}
			else printf("%d\t|",0);
		}
        printf("\n\t|");
		for (int i = 0; i < psize; i++){
			printf("\t|");
		}
        printf("\nTime:\t|");
        for (int i = 0; i < psize; i++) {
			if (car_park[i]) {
				printf("%d\t|",(int)difftime(car_park[i]->ltm, time(NULL)));
			}
			else printf("%d\t|",0);
		}
		printf("\n");
		sleep((rand() % 20)/100.0);              // get a random value between 0 and 0.2
		// === Exit CS for park ====
        
        if(!AisEmpty()){
        Car* car = Apeek();
        double diff = difftime(car->ltm, time(NULL));
        if(diff <= 0)
            pthread_cond_signal(&outVcond);
        }
        // Calculate the utilization as a percentage
        ut = (double) Asize() / (double) Acapacity() * 100.0;

        pthread_mutex_unlock(&park_lock);		 // Unlock the arrival queue
		printf("-------------------------------------------------------------------\n");
		prev_t = current_t;
        times_monitored++;
        
        tot_ut += ut; // add to total utilization for finding average utilization later
        updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
		show();
        sleep(1);
    }
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
        if (psize_arg > PSIZE_MAX) {
            printf("Invalid park size. Using Max value %d\n", PSIZE_MAX);
            psize = PSIZE_MAX;
        } else if (psize_arg < PSIZE_MIN) {
            printf("Invalid park size. Using Min value %d\n", PSIZE_MIN);
            psize = PSIZE_MIN;
        } else {
            psize = psize_arg;
        }
    }
    if (argc >= 3) {
        inval_arg = atoi(argv[2]);
        if (inval_arg > IN_VAL_MAX) {
            printf("Invalid number of in-valets. Using Max value %d\n", IN_VAL_MAX);
            inval = IN_VAL_MAX;
        } else if (inval_arg < IN_VAL_MIN) {
            printf("Invalid number of in-valets. Using Min value %d\n", IN_VAL_MIN);
            inval = IN_VAL_MIN;
        } else {
            inval = inval_arg;
        }
    }
    if (argc >= 4) {
        outval_arg = atoi(argv[3]);
        if (outval_arg > OUT_VAL_MAX) {
            printf("Invalid number of out-valets. Using Max value %d\n", OUT_VAL_MAX);
            outval = OUT_VAL_MAX;
        } else if (outval_arg < OUT_VAL_MIN) {
            printf("Invalid number of out-valets. Using Min value %d\n", OUT_VAL_MIN);
            outval = OUT_VAL_MIN;
        } else {
            outval = outval_arg;
        }
    }
    if (argc >= 5) {
        qsize_arg = atoi(argv[4]);
        if (qsize_arg > QSIZE_MAX) {
            printf("Invalid queue size. Using Max value %d\n", QSIZE_MAX);
            psize = PSIZE_MAX;
            qsize = QSIZE_MAX;
        } else if (qsize_arg < QSIZE_MIN) {
            printf("Invalid queue size. Using Min value %d\n", QSIZE_MIN);
            qsize = QSIZE_MIN;
        } else {
            qsize = qsize_arg;
        }
    }
    if (argc >= 6) {
        expnum_arg = atof(argv[5]);
        if (expnum_arg > EXPNUM_MAX) {
            printf("Invalid expected number of cars. Using Max value %lf\n", EXPNUM_MAX);
            expnum = EXPNUM_MAX;
        } else if (expnum_arg < EXPNUM_MIN) {
            printf("Invalid expected number of cars. Using Min value %lf\n", EXPNUM_MIN);
            expnum = EXPNUM_MIN;
        } else {
            expnum = expnum_arg;
        }
    }

    printf("Car park size: %d\n", psize);
    printf("Number of in valets: %d\n", inval);
    printf("Number of out valets: %d\n", outval);
    printf("Queue size: %d\n", qsize);
    printf("Expected number of cars: %f\n", expnum);
    //===============end arg======================================

    // Initialize mutexes
    pthread_mutex_init(&park_lock, NULL);
    pthread_mutex_init(&queue_lock, NULL);

    // Initialize cond var
    pthread_cond_init(&outVcond, NULL);

    // Parking Sem
    sem_init(&P_emptySlots, 0, psize);
    sem_init(&P_haveCars, 0, 0);

    // Queue Sem
    sem_init(&Q_emptyQueue, 0, qsize);
    sem_init(&Q_haveCars, 0, 0);


    // Initialize the car park and its components
    Qinit(qsize);   // initialize Queue
    // pQinit(psize);  // initialize priority Queue
    Ainit(psize);   // initialize Park Array

    car_park_array = Aiterator(&psize);
    G2DInit(car_park_array, psize, inval, outval, park_lock);
    show();
    sleep(0.2);

    // Set up the signal handler for SIGTERM to abort program
    signal(SIGINT, sigterm_handler);
    signal(SIGQUIT, sigterm_quit);

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
            printf("Failed to Create In Valet Thread #%d\n", *id);
            return 1;
        }
    }
    in_valet_threads_id = in_valet_threads;

    pthread_t out_valet_threads[outval];
    for(int i = 0; i < outval; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&out_valet_threads[i], &attr, out_valet_func, id) != 0) {
            printf("Failed to Create Out Valet Thread #%d\n", *id);
            return 1;
        }
    }
    out_valet_threads_id = out_valet_threads;

    // Create the car generator thread
    pthread_t car_gen_thread;
    double* prob = malloc(sizeof(double));
    *prob = expnum;
    if (pthread_create(&car_gen_thread, &attr, car_gen_func, prob) !=0) {
        printf("Failed to Create Car Generator Thread\n");
        return 1;
    }
    car_gen_thread_id = car_gen_thread;

    if (pthread_join(car_gen_thread, NULL) != 0) {
        printf("Finished Simulation");
        return 2;
    }
    // testCP();
}


