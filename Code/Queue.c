#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"
#include "Car.h"

Queue q;

void Qinit(int n) {
    q.data = (Car**) malloc(n * sizeof(Car*));
    q.list = (Car**) malloc(n * sizeof(Car*));
    q.capacity = n;
    q.count = 0;
    q.tail = 0;
    q.head = 0;
}

void Qfree() {
    free(q.data);
    free(q.list);
}

void Qclear() {
    q.count = 0;
    q.tail = 0;
    q.head = 0;
}

void Qenqueue(Car *car) {
    if (q.count < q.capacity) {
        q.data[q.tail] = car;
        q.tail = (q.tail + 1) % q.capacity;
        q.count++;
    }
}

Car* Qserve() {
    Car* car = NULL;
    if (q.count > 0) {
        car = q.data[q.head];
        q.head = (q.head + 1) % q.capacity;
        q.count--;
    }
    return car;
}

Car* Qpeek() {
    Car* car = NULL;
    if (q.count > 0) {
        car = q.data[q.head];
    }
    return car;
}

Car** Qiterator(int *sz) {
    int i;
    for (i = 0; i < q.count; i++) {
        q.list[i] = q.data[(q.head + i) % q.capacity];
    }
    *sz = q.count;
    return q.list;
}

int Qcapacity() {
    return q.capacity;
}

int Qsize() {
    return q.count;
}

bool QisFull() {
    return q.count == q.capacity;
}

bool QisEmpty() {
    return q.count == 0;
}