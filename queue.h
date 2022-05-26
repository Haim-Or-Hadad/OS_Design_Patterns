
#include <iostream>
using namespace std;

struct node
{   
void *data;
node *next;
};

struct queue
{
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
node * head;
node * tail;
size_t size;
};
void* deQ(struct queue *q);
struct queue *createQ()
{
    
queue * q = (struct queue*)malloc(sizeof(struct queue));
pthread_mutex_init(&q->mutex, NULL);
pthread_cond_init(&q->cond, NULL);
q->head = nullptr;
q->tail = nullptr;
return q;
}
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
//   enQ(q,&c);
//   cout << q->head->data << endl;
// return 0;
// }