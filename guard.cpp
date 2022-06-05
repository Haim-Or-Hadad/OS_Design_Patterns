#include <iostream>
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


int main()
{
    std::cout << "ilan test";
}

/*
In the first call to strtok, you supply the string and the delimiters.
In subsequent calls, the first parameter is NULL, and you just supply the delimiters.
strtok remembers the string that you passed in.
In a multithreaded environment, this is dangerous because many threads may be calling
 strtok with different strings. It will only remember the last one and return the wrong 
 result.
*/
//https://stackoverflow.com/questions/20820937/strtok-function-and-multithreading