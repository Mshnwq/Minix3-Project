/*
 * parkArray.h
 *
 * Header file for a FIFO queue structure using an array of
 * Car-type elements stored in it.
 *
 * The following function specifications shall be used to
 * implement the FIFO queue:
 *
 * - void  Ainit(int n)
 * - void  Afree()
 * - void  Aclear()
 * - void  Aenqueue(Car *car)
 * - Car*  Aserve()
 * - Car*  Apeek()
 * - Car** Aiterator(int *sz)
 * - int   Acapacity()
 * - int   Asize()
 * - bool  AisFull()
 * - bool  AisEmpty()
 *
 * Author Tamam Alahdal
 * Version 3.00 2023/02/9
 *
 */

#include <pthread.h>		// Provides thread-safe mutex lock
#include <stdbool.h>        // Provides boolean data type
#include "Car.h"
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
typedef struct Array_t {
    Car **data;       		// Array to hold car queue
	Car **list;				// Array to hold car list
    int capacity;      		// The array (queue) capacity
    int count;              // Number of cars currently in the queue
    int front;             	// pointer to the array will do the work of insertion and deletion.
}Array;


/* =============================================================================
 * Initialize the feilds of a Queue structure instance.
 * =============================================================================
 */
void Ainit(int n);

/* =============================================================================
 * Free the Queue data and list arrays.
 * =============================================================================
 */
void Afree();

/* =============================================================================
 * Clear the Queue.
 * =============================================================================
 */
void Aclear();

/* =============================================================================
 * A FIFO Queue enqueue function.
 * Check precondition Qisfull() = false.
 * =============================================================================
 */
int Aenqueue(Car *car);

/* ===========================================================================
 * A FIFO Queue delete function.
 * Delete and return the car at Queue head.
 * Check precondition QisEmpty = false.
 * ===========================================================================
 */
Car* Aserve(int slotN);

/* ===========================================================================
 * Return the car at the head of the Queue, without deleting it.
 * ===========================================================================
 */
Car* Apeek();

/* ===========================================================================
 * Return a list of the queue contents and its size.
 * ===========================================================================
 */
Car** Aiterator(int *sz);

/* ===========================================================================
 * Return the capacity of the FIFO Queue.
 * ===========================================================================
 */
int Acapacity();

/* ===========================================================================
 * Return the number of cars in the FIFO Queue.
 * ===========================================================================
 */
int Asize();

/* ===========================================================================
 * Return true if the FIFO Queue is full. Return false otherwise.
 * ===========================================================================
 */
bool AisFull();

/* ===========================================================================
 * Return true if the FIFO Queue is empty. Return false otherwise.
 * ===========================================================================
 */
bool AisEmpty();


Car* minimum(Car* car1, Car* car2);