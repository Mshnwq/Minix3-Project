#include <stdlib.h>
#include <stdio.h>
#include "pQueue.h"
#include "Car.h"

// #ifndef CAR_H
// #define CAR_H


pQueue parking;

void pQinit(int n) {
    parking.data = (Car**) malloc(n * sizeof(Car*));
    parking.list = (Car**) malloc(n * sizeof(Car*));
    parking.capacity = n;
    parking.count = 0;
    parking.front = 0;
}

void pQfree() {
    free(parking.data);
    free(parking.list);
}

void pQclear() {
    parking.count = 0;
}

int pQenqueue(Car *newCar) {
    int slotN;
    if (!pQisFull()) {
        int counter = 0;
        Car* temp = parking.data[counter];
        bool notParked = true;
        while(counter < parking.front){
             if(compare(newCar,temp)){ //if true means the new car has time less then current car
                parking.data[counter] = newCar;
                slotN = counter;
                notParked = false;
            //shift
                while(counter < parking.front-1){
                    Car* temp2 = parking.data[++counter];
                    parking.data[counter] = temp;
                    temp = temp2;   
                    free(temp2);
                }
                parking.data[parking.front] = temp;
                break;
            }
            else{
                temp = parking.data[++counter];
            }
        }
        //if new Car have highest ltm then parking will at the front 
        if(notParked){
            parking.data[parking.front++] = newCar;
        }
        free(temp);
        parking.count++;
        parking.front++;
    }
    return slotN;
}

Car* pQserve() {
    Car* car = NULL;
    if (!pQisEmpty()) {
        car = parking.data[--parking.front];
    }
    return car;
}

Car* pQpeek() {
    Car* car = NULL;
    if (parking.count > 0) {
        car = parking.data[parking.front];
    }
    return car;
}

Car** pQiterator(int *sz) {
    int i;
    for (i = 0; i < parking.count; i++) {
        parking.list[i] = parking.data[i];
    }
    //why?
    *sz = parking.count;
    return parking.list;
}

int pQcapacity() {
    return parking.capacity;
}

int pQsize() {
    return parking.count;
}

bool pQisFull() {
    return parking.count == parking.capacity;
}

bool pQisEmpty() {
    return parking.count == 0;
}

bool compare(Car* newCar, Car* car1){
    if(newCar->ltm < car1->ltm){
        return true;
    }
    else{
        return false;
    }
}
// #endif