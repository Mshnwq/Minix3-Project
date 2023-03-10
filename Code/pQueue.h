/*
 * pQueue.h
 *
 * Header file for a FIFO queue structure using an array of
 * Car-type elements stored in it.
 *
 * The following function specifications shall be used to
 * implement the FIFO queue:
 *
 * - void  pQinit(int n)
 * - void  pQfree()
 * - void  pQclear()
 * - void  pQenqueue(Car *car)
 * - Car*  pQserve()
 * - Car*  pQpeek()
 * - Car** pQiterator(int *sz)
 * - int   pQcapacity()
 * - int   pQsize()
 * - bool  pQisFull()
 * - bool  pQisEmpty()
 *
 * Author Tamam Alahdal
 * Version 3.00 2023/02/9
 *
 */

#include <pthread.h>		// Provides thread-safe mutex lock
#include <stdbool.h>        // Provides boolean data type
// #include "CarPark.h"
#include "Car.h"
// #ifndef CAR_H
// #define CAR_H
/* =============================================================================
 * FIFO Queue struct holds the queue array and its standard field variables.
 * To implement this queue write the standard queue operations with signatures
 * as listed below. [Qinit(), Qfree(), Qenqueue(), Qserve(), Qpeek(), Qsize(),
 * Qcapacity(), Qclear(), QisEmpty(), and QisFull()].
 *
 * Also use the extra field "list" to write function [Qiterator] that returns
 * a list used to traverse all the elements of the queue starting at the head
 * and ending at the tail without changing the state of the queue.
 * =============================================================================
 */
typedef struct pQueue_t {
    Car **data;       		// Array to hold car queue
	Car **list;				// Array to hold car list
    int capacity;      		// The array (queue) capacity
    int count;              // Number of cars currently in the queue
    int front;             	// points to  the last element of the priority queue
}pQueue;

// pQueue parking;
// Car car;
/* =============================================================================
 * Initialize the feilds of a Queue structure instance.
 * =============================================================================
 */
void pQinit(int n);

/* =============================================================================
 * Free the Queue data and list arrays.
 * =============================================================================
 */
void pQfree();

/* =============================================================================
 * Clear the Queue.
 * =============================================================================
 */
void pQclear();

/* =============================================================================
 * A FIFO Queue enqueue function.
 * Check precondition Qisfull() = false.
 * =============================================================================
 */
int pQenqueue(Car *car);

/* ===========================================================================
 * A FIFO Queue delete function.
 * Delete and return the car at Queue head.
 * Check precondition QisEmpty = false.
 * ===========================================================================
 */
Car* pQserve();

/* ===========================================================================
 * Return the car at the head of the Queue, without deleting it.
 * ===========================================================================
 */
Car* pQpeek();

/* ===========================================================================
 * Return a list of the queue contents and its size.
 * ===========================================================================
 */
Car** pQiterator(int *sz);

/* ===========================================================================
 * Return the capacity of the FIFO Queue.
 * ===========================================================================
 */
int pQcapacity();

/* ===========================================================================
 * Return the number of cars in the FIFO Queue.
 * ===========================================================================
 */
int pQsize();

/* ===========================================================================
 * Return true if the FIFO Queue is full. Return false otherwise.
 * ===========================================================================
 */
bool pQisFull();

/* ===========================================================================
 * Return true if the FIFO Queue is empty. Return false otherwise.
 * ===========================================================================
 */
bool pQisEmpty();


bool compare(Car* newCar, Car* car1);
// #endif