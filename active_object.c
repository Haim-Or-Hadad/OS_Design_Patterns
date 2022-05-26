#include "active_object.h"
#include <pthread.h>



/*********create new active object******/
AO * newAO(struct queue* q,  void* func_1, void* func_2)
{
AO active_object_ = (AO)malloc(sizeof(AO));
active_object_->q = q;
active_object_->function_1 = func_1;
active_object_->function_2 = func_2;
//active_object_->thread_ = (pthread_t *)(malloc(sizeof(pthread_t)));
//pthread_create(active_object_->thread_, NULL, run_AO, (void*)(active_object_));
active_object_->running = 1;
return active_object_;
}

void run_AO()
{

}

void destroy(AO ao)
{
destroyQ(ao->q);
ao->running = 0;
//free(ao->thread_);
pthread_cancel(ao->id);
free(ao);
}