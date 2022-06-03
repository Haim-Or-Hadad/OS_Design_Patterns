#pragma once
#include "active_object.h"
#include <pthread.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef struct queue queue;


/*********create new active object******/
AO * newAO(struct queue* q,  void* func_1, void* func_2)
{
AO* active_object_ = (AO*)malloc(sizeof(AO));
active_object_->q = q;
active_object_->function_1 = func_1;
active_object_->function_2 = func_2;
active_object_->id=(pthread_t *)(malloc(sizeof(pthread_t)));
pthread_create(active_object_->id, NULL, run_AO, (void*)(active_object_));
active_object_->running = 1;
return active_object_;
}

/**
 * @brief run the active object functions.
 */
void* run_AO(void* ao)
{
    AO *new_ao=(AO *) ao;
    while(new_ao->running){
        void* First_result =new_ao->function_1(deQ(new_ao->q));
        char *new_node=(char *) First_result;
        void* sescond_result=new_ao->function_2(First_result);
    }
    free(new_ao->id);
    free(new_ao);
}

/**
 * @brief Destroy the active object
 * 
 * @param ao1 
 */
void destroyAO(void* ao1)
{
    AO *ao=(AO *) ao1;
    ao->running = 0;
    pthread_cancel(ao->id);
    free(ao->id);
    free(ao);
}


// void* Caesar_Cipher(void* word){\
//     struct node_data* data=(struct node_data*)word;
//     char* curr_char=(char*)data->data;
//     for (size_t i = 0; i < strlen(curr_char); i++)
//     {
//         if (curr_char[i] == 'Z')
//         {
//              data->data[i] = 'A';
//         }
//         else if (curr_char[i] =='z')
//         {
//              data->data[i] = 'a';
//         }
//         else if(curr_char[i]>=65 && curr_char[i]<122){
//              data->data[i] = curr_char[i]+1;
//         }
//     }
//     return data;
// }

// void* Change_letter_case(void* word){
//     struct node_data* data=(struct node_data*)word;
//     char* curr_char=(char*)data->data;
//     for (size_t i = 0; i < strlen(curr_char); i++)
//     {
//         if ((curr_char[i] >= 65) && (curr_char[i] <= 90)){
//             data->data[i]=tolower(curr_char[i]);
//         }
//         else if((curr_char[i] >= 97) && (curr_char[i] <= 122)){
//             data->data[i]=toupper(curr_char[i]);
//         }
//     }
//     return data;
    
// }