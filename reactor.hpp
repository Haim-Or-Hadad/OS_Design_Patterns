#include <pthread.h>
#pragma once

struct Reactor
{
    void *(*function_1)(void*);
    int fd;
    pthread_t thread;
};


void* newReactor();
void InstallHandler(Reactor* react,void* func_1(void*),int* fd);
void RemoveHandler(Reactor* react);
