#include "active_object.h"
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct queue queue;

void* Caesar_Cipher(void* word){
    // printf("%ld",sizeof(word));
    char* curr_char=(char*)word;
    char* new_word=malloc(sizeof((char*)word));
    for (size_t i = 0; i < strlen(curr_char); i++)
    {
        if (curr_char[i] == 'Z')
        {
            new_word[i] = 'A';
        }
        else if (curr_char[i] =='z')
        {
            new_word[i] = 'a';
        }
        else{
            new_word[i] += curr_char[i]+1;
        }
    }

    return new_word;
}

void* Change_letter_case(void* word){
    char* curr_char=(char*)word;
    char* new_word=malloc(sizeof((char*)word));
    for (size_t i = 0; i < strlen(word); i++)
    {
        if ((curr_char[i] >= 65) && (curr_char[i] <= 90)){
            new_word[i]=tolower(curr_char[i]);
        }
        else{
            new_word[i]=toupper(curr_char[i]);
        }
    }
    return new_word;
    
}

/*********create new active object******/
AO * newAO(struct queue* q,  void* func_1, void* func_2)
{
AO* active_object_ = (AO*)malloc(sizeof(AO));
active_object_->q = q;
active_object_->function_1 = func_1;
active_object_->function_2 = func_2;
active_object_->id = (pthread_t *)(malloc(sizeof(pthread_t)));
pthread_create(active_object_->id, NULL, run_AO, (void*)(active_object_));
active_object_->running = 1;
return active_object_;
}

void* run_AO(void* ao)
{
    AO *new_ao=(AO *) ao;
     printf("before\n");
    void* First_result =new_ao->function_1(deQ(new_ao->q));
    printf("after %s\n", (char*)First_result);
    char *new_node=(char *) First_result;
    printf("%s\n",new_node);
    void* sescond_result=new_ao->function_2(First_result);
    printf("after2 %s\n", (char*)sescond_result);
    // free(ao->id);
    // free(ao);
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
    AO *firstAO;
    firstQueue = createQ();
    enQ(firstQueue,"HELLO");
    //******************************************************* dont work with AO
    //  struct node* t;
    //  t->data=deQ(firstQueue);
    //  printf("%s ",t->data);
    //  struct node * test1=Caesar_Cipher(t);
    //  printf("%s ",test1->data);
    //*******************************************************
    firstAO = newAO(firstQueue, Caesar_Cipher,Change_letter_case);
    sleep(14);
    //run_AO(firstAO);
    // //destroyAO(firstAO);
    // destroyQ(firstQueue);
    return 0;
}
