
#include <stdbool.h>
#include <pthread.h>


struct node
{   
void* data;
int socket_fd;
struct node *next;
}node;

 struct queue
{
pthread_cond_t cond; //= PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER;
struct node* head;
struct node* tail;
size_t size;
};

void* deQ(struct queue *q);
struct queue *createQ();
void destroyQ(struct queue *q);
void enQ(struct queue *q,void *data);
void* deQ(struct queue *q);
