#include <pthread.h>
#include <iostream>
#include "reactor.hpp"

void* newReactor(){
    Reactor* new_react= (Reactor*)(malloc(sizeof(Reactor)));
    return new_react;
}

void InstallHandler(Reactor* react,void* func_1(void*),int* fd){
    react->fd=*fd;
    react->function_1=func_1;
    pthread_create(&react->thread,NULL,func_1,(void*) fd);
}

void RemoveHandler(Reactor* react){
    pthread_cancel(react->thread);
    free(react);
}


