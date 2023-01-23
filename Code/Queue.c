#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"
// #ifndef Car_H
// #define Car_H

void Qinit(int n){
	// carsQ.capacity = n;
	// carsQ.count = 0;
	// carsQ.head = 0;
	// carsQ.tail = n-1;
	// carsQ.data =(Car**) calloc(n, sizeof(Car*));
	// carsQ.list =(Car**) calloc(0, sizeof(Car*));	
}


int Qcapacity(){
	// return carsQ.capacity;
}


int Qsize(){
	// return carsQ.count;
}


void Qfree(){
    // carsQ.count = 0;
	// free(carsQ.data);
	// free(carsQ.list);
}


void Qclear(){
	// carsQ.count = 0;
	// carsQ.head = 0;
	// carsQ.tail = Qcapacity()-1;
	// carsQ.list =(Car**) realloc(carsQ.list, Qsize()*sizeof(Car*));
}


bool QisEmpty(){
	// return carsQ.count == 0;
}


bool QisFull(){
	// return carsQ.count == Qcapacity();
}


void Qenqueue(Car *car){
	// if (!QisFull()){
	// 	carsQ.tail = (carsQ.tail+1)%Qcapacity();
	// 	carsQ.data[carsQ.tail] = car;
	// 	carsQ.count++;
	// 	carsQ.list =(Car**) realloc(carsQ.list, Qsize()*sizeof(Car*));
	// 	carsQ.list[Qsize()-1] = car;
	// }
}


Car* Qserve(){
	// if (!QisEmpty()){
	// 	Car *temp = carsQ.data[carsQ.head];
	// 	carsQ.head = (carsQ.head+1)%Qcapacity();
	// 	//carsQ.list[0] = carsQ.list[Qsize()-1];
	// 	carsQ.count--;
	// 	for (int i = 0; i < Qsize(); i++)
	// 		carsQ.list[i] = carsQ.list[i+1];
	// 	carsQ.list =(Car**) realloc(carsQ.list, Qsize()*sizeof(Car*));
	// 	return temp;
	// }
	// return NULL;
}

Car* Qpeek(){
	// if (!QisEmpty()){
	// 	return carsQ.data[carsQ.head];
	// }
	// return NULL;
}


Car** Qiterator(int *sz){
	// *sz = Qsize();
	// return carsQ.list;	
}
// #endif