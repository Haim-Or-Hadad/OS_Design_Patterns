#include "active_object.h"
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct queue queue;

void Caesar_Cipher(char* word){
    for (size_t i = 0; i < strlen(word); i++)
    {
        if (word[i] == 'Z')
        {
            word[i] = 'A';
        }
        else if (word[i] =='z')
        {
            word[i] = 'a';
        }
        else{
            word[i] += 1;
        }
    }
    
}

void Change_letter_case(char* word){
    for (size_t i = 0; i < strlen(word); i++)
    {
        if ((word[i] >= 65) && (word[i] <= 90)){
            word[i]=tolower(word[i]);
        }
        else{
            word[i]=toupper(word[i]);
        }
    }
    printf("%s\n",word);
    
}

/*********create new active object******/
AO * newAO(struct queue* q,  void* func_1, void* func_2)
{
AO* active_object_ = (AO*)malloc(sizeof(AO));
active_object_->q = q;
active_object_->function_1 = func_1;
active_object_->function_2 = func_2;
//active_object_->thread_ = (pthread_t *)(malloc(sizeof(pthread_t)));
//pthread_create(active_object_->thread_, NULL, run_AO, (void*)(active_object_));
active_object_->running = 1;
return active_object_;
}

void run_AO(AO* ao)
{
    void* handled_now =ao->function_1(deQ(ao->q));
    void* next=ao->function_2(handled_now);
    //free(ao->id);
    free(ao);
}

void destroy(AO* ao)
{
destroyQ(ao->q);
ao->running = 0;
//free(ao->thread_);
pthread_cancel(ao->id);
free(ao);
}


int main(int argc, char const *argv[])
{
queue* firstQueue;
queue* secondQueue;
queue* thirdQueue;
AO *firstAO;
AO *secondAO; 
AO *thirdAO;
    firstQueue = createQ();
    // secondQueue = createQ();
    // thirdQueue = createQ();
    enQ(firstQueue,"HELLO");
    firstAO = newAO(firstQueue, Caesar_Cipher,Change_letter_case);
    run_AO(firstAO);
    //destroyAO(firstAO);
    destroyQ(firstQueue);
    return 0;
}
