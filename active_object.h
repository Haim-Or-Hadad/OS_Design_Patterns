#include "queue.h"
#include <pthread.h>


/*active_object have queue of elements and two pointers to functions*/
typedef struct active_object
{
    struct queue* q;
    void *(*function_1)(void*);//function_1 is a pointer to function taking one argument, an void and return void
    void *(*function_2)(void*);//function_2 is a pointer to function taking one argument, an void and return void
    pthread_t id;//new thread to the active object
    int running;
}AO;


AO * newAO(struct queue* q,  void* func_1, void* func_2);
void run_AO();
void destroyAO(AO* ao);
