#include "Queue.h"
#include <pthread.h>


/*active_object have queue of elements and two pointers to functions*/
typedef struct active_object
{
    queue* q;
    void *(*function_1)(void*);//function_1 is a pointer to function taking one argument, an void and return void
    void *(*function_2)(void*);//function_2 is a pointer to function taking one argument, an void and return void
    pthread_t *thread_;//new thread to the active object
}AO;


AO* newAO(struct queue* q,  void* func_1, void* func_2);
void destroyAO();
