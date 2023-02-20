#include <stdlib.h>
#include <stdio.h>
#include "parkArray.h"
#include "Car.h"

Array parking1;
bool* a;
void Ainit(int n) {
    parking1.list = (Car**) malloc(n * sizeof(Car*));
    a = (bool *) malloc(n*sizeof(bool)); //true if car is park there 
    
    for(int i=0; i<n; i++){
        a[i] = false;
    }
    parking1.capacity = n;
    parking1.count = 0;
    parking1.front = 0;
}

void Afree() {
    free(parking1.data);
    free(parking1.list);
}

void Aclear() {
    parking1.count = 0;
}

int Aenqueue(Car *newCar) {
    //seerch for a free place in the park
    int soltN;
    if (Asize() < Acapacity()) {
        parking1.front = rand()%Acapacity();
        while(1){
            //to check if slot is empyt
            if(a[parking1.front]){
                parking1.front = rand()%Acapacity();//(parking1.front) % parking1.capacity; //circle move
            }
            else{
                //to park the car if slot is empty
                parking1.list[parking1.front] = newCar;
                parking1.count++;
                a[parking1.front] = true;
                soltN = parking1.front;
                break;
            }
        }
    }
    return soltN;
    
}

Car* Aserve(int SlotN) {

    Car* min = parking1.list[SlotN]; 

    a[SlotN] = false;
    parking1.list[SlotN] = NULL;
    parking1.count--;
   return min;

}

Car* Apeek() {
    Car* min;  
    int i=0;
    while(!a[i]){
        i++;
    }  
    min = parking1.list[i];
    for(int i=0; i < parking1.capacity; i++){
        if(a[i]){
            min = minimum(min,parking1.list[i]);
        }
    }

    return min;
}

Car** Aiterator(int *sz) {
    int i;
    *sz = Acapacity();
    for (i = 0; i < parking1.capacity; i++) {
        parking1.list[i] = parking1.list[i];
    }
    return parking1.list;
}

int Acapacity() {
    return parking1.capacity;
}

int Asize() {
    return parking1.count;
}

bool AisFull() {
    return parking1.count == parking1.capacity;
}

bool AisEmpty() {
    return parking1.count == 0;
}

Car* minimum(Car* car1, Car* car2){
    if(car1->ltm < car2->ltm)
        return car1;
    else
        return car2;

}