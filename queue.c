#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******create of queue**********/
struct queue *createQ()
{
queue * q = (struct queue*)malloc(sizeof(struct queue));
pthread_mutex_init(&q->mutex, NULL);
pthread_cond_init(&q->cond, NULL);
q->head = NULL;
q->tail = NULL;
return q;
}

/*******delete the queue**********/
void destroyQ(struct queue *q)
{
    for (int i = 0; i < q->size ; i++)
    {
        deQ(q);
    }
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    free(q);   
}

/*******insert element to the queue**********/
void enQ(struct queue *q,void *data)
{
    pthread_mutex_lock(&q->mutex);
    node *new_node = (struct node*)malloc(sizeof(node*));
    new_node->data = data;
    if (q->size == 0 ){
        q->head = new_node;
        q->tail = new_node;
        q->size++;
    }
    else
    {
        q->tail->next=new_node;
        q->tail=new_node;
        q->size++;
    } 
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

/*******delete element from the queue**********/
void* deQ(struct queue *q)
{
    pthread_mutex_lock(&q->mutex);
    while(q->size == 0)
    {
    pthread_cond_wait(&q->cond, &q->mutex);
    }
    void * en_q = q->head->data;
    node *temp= q->head;
    q->head = q->head->next;
    free(temp);
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return en_q;
}


// int main(){
//   queue *q = createQ();
//   int c=3;
//   cout << q->size << endl;
//   enQ(q,&c);
//   enQ(q,&c);
//   enQ(q,&c);
//   enQ(q,&c);
//   cout << q->size << endl;
//   cout << deQ(q) << endl;
//   cout << q->size << endl;
//   destroyQ(q);
// return 0;
// }
