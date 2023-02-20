#include <stdlib.h>
#include <stdio.h>
#include "Queue.h"
#include "Car.h"
//why?
// #ifndef Car_H
// #define Car_H

Queue q;
// pthread_mutex_t lock;

void Qinit(int n) {
    q.data = (Car**) malloc(n * sizeof(Car*));
    q.list = (Car**) malloc(n * sizeof(Car*));
    q.capacity = n;
    q.count = 0;
    q.tail = 0;
    q.head = 0;
    // pthread_mutex_init(&lock, NULL);
}

void Qfree() {
    free(q.data);
    free(q.list);
    // pthread_mutex_destroy(&lock);
}

void Qclear() {
    // pthread_mutex_lock(&lock);
    q.count = 0;
    q.tail = 0;
    q.head = 0;
    // pthread_mutex_unlock(&lock);
}

void Qenqueue(Car *car) {
    // pthread_mutex_lock(&lock);
    if (q.count < q.capacity) {
        q.data[q.tail] = car;
        q.tail = (q.tail + 1) % q.capacity;
        q.count++;
    }
    // pthread_mutex_unlock(&lock);
}

Car* Qserve() {
    // pthread_mutex_lock(&lock);
    Car* car = NULL;
    if (q.count > 0) {
        car = q.data[q.head];
        q.head = (q.head + 1) % q.capacity;
        q.count--;
    }
    // pthread_mutex_unlock(&lock);
    return car;
}

Car* Qpeek() {
    Car* car = NULL;
    // pthread_mutex_lock(&lock);
    if (q.count > 0) {
        car = q.data[q.head];
    }
    // pthread_mutex_unlock(&lock);
    return car;
}

Car** Qiterator(int *sz) {
    int i;
    // pthread_mutex_lock(&lock);
    for (i = 0; i < q.count; i++) {
        q.list[i] = q.data[(q.head + i) % q.capacity];
    }
    *sz = q.count;
    // pthread_mutex_unlock(&lock);
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
// #endif