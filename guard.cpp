#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <thread>

pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

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

int i=0;
int c;
void *print_message_function( void *ptr )
{
    guard g(&lock2);
    std::cout << "thread "<< i << " start" << std::endl;
    for (int i = 0; i < 25; i++)
    {
        c++;
        (char*) ptr; // update ptr
    } 
    std::cout << "thread "<< i << " done"<< std::endl;
    i++; 
}



int main()
{
     pthread_t thread1, thread2;
     std::string m1 = "Thread 1";
     std::string m2 = "Thread 2";
     pthread_create( &thread1, NULL, print_message_function, &m1);
     pthread_create( &thread2, NULL, print_message_function, &m2);
     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 
     std::cout << "Thread 1 returns: " << thread1 << std::endl;
     std::cout << "Thread 2 returns: " << thread2 << std::endl;
     exit(0);
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
