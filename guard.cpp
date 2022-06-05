#pragma once 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//exercise 5
class guard
{
private:
pthread_mutex_t * lock;
public:
    guard(pthread_mutex_t * lock);
    ~guard();
};

guard::guard(pthread_mutex_t * new_lock) 
{ 
 this->lock = new_lock;
 pthread_mutex_lock(this->lock);
}

guard::~guard()
{
pthread_mutex_unlock(this->lock);
}
