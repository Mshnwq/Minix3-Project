#include <stdlib.h>
#include <stdio.h>
#include "pQueue.h"
#include "Car.h"

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
        Car* temp = parking.list[counter];
        bool notParked = true;
        while(counter < parking.front){
             if(compare(newCar, temp)){ //if true means the new car has time less then current car then park
                parking.list[counter] = newCar;
                slotN = counter;
                notParked = false;
                //shift cars
                while(counter < parking.front-1){
                    Car* temp2 = parking.list[++counter];
                    parking.list[counter] = temp;
                    temp = temp2;   
                }
                parking.list[parking.front] = temp;
                break;
            }
            else{
                temp = parking.list[++counter];
            }
        }
        //if new Car have highest ltm then parking will at the front 
        if(notParked){
            parking.list[parking.front] = newCar;
            slotN = parking.front;
        }
        parking.count++;
        parking.front++;

    }
    return slotN;
}

Car* pQserve() {
    Car* car;
    // printf("car not served yet\n");
    if (!pQisEmpty()) {
        // printf("car is served\n");
        car = parking.list[--parking.front];
        parking.count--;
    }
    return car;
}

Car* pQpeek() {
    Car* car;
    if (!pQisEmpty()) {
        car = parking.list[parking.front-1];
    }
    return car;
}

Car** pQiterator(int *sz) {
    int i;
    *sz = pQcapacity();
    for (i = 0; i < parking.capacity; i++) {
        parking.list[i] = parking.list[i];
    }
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

bool compare(Car* newCar, Car* oldCar){
    if(difftime(newCar->ltm, time(NULL)) > difftime(oldCar->ltm, time(NULL))){
        return true;
    }
    else{
        return false;
    }
}
