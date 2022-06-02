/*
** server.c -- a stream socket server demo
*/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>


#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold

/**************************Queue functions************************/
struct node
{   
void* data;
struct node *next;
}node;

struct node_data{
    char data[2048];
    int sock;
}node_data;

struct queue
{
pthread_cond_t cond; //= PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex;// = PTHREAD_MUTEX_INITIALIZER;
struct node* head;
struct node* tail;
size_t size;
};

/*******create of queue**********/
struct queue *createQ()
{
struct queue * q = (struct queue*)malloc(sizeof(struct queue));
pthread_mutex_init(&q->mutex, NULL);
pthread_cond_init(&q->cond, NULL);
q->head = ((void*)0);
q->tail = ((void*)0);
return q;
}

/*******insert element to the queue**********/
void enQ(struct queue *q,void *data)
{
    pthread_mutex_lock(&q->mutex);
    struct node *new_node = (struct node*)malloc(sizeof(node));
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
    struct node *temp= q->head;
    q->head = q->head->next;
    free(temp);
    q->size--;
    pthread_mutex_unlock(&q->mutex);
    return en_q;
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
/*****************************End of Queue functions******************************************************************/

/**************************active object functions************************/
typedef struct active_object
{
    struct queue* q;
    void *(*function_1)(void*);//function_1 is a pointer to function taking one argument, an void and return void
    void *(*function_2)(void*);//function_2 is a pointer to function taking one argument, an void and return void
    pthread_t id;//new thread to the active object
    int running;
}AO;

void* Caesar_Cipher(void* word){\
    struct node_data* data=(struct node_data*)word;
    char* curr_char=(char*)data->data;
    for (size_t i = 0; i < strlen(curr_char); i++)
    {
        if (curr_char[i] == 'Z')
        {
             data->data[i] = 'A';
        }
        else if (curr_char[i] =='z')
        {
             data->data[i] = 'a';
        }
        else if(curr_char[i]>=65 && curr_char[i]<122){
             data->data[i] = curr_char[i]+1;
        }
    }
    return data;
}

void* Change_letter_case(void* word){
    struct node_data* data=(struct node_data*)word;
    char* curr_char=(char*)data->data;
    for (size_t i = 0; i < strlen(curr_char); i++)
    {
        if ((curr_char[i] >= 65) && (curr_char[i] <= 90)){
            data->data[i]=tolower(curr_char[i]);
        }
        else if((curr_char[i] >= 97) && (curr_char[i] <= 122)){
            data->data[i]=toupper(curr_char[i]);
        }
    }
    return data;
    
}

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

void destroyAO(void* ao1)
{
    AO *ao=(AO *) ao1;
    ao->running = 0;
    pthread_cancel(ao->id);
    free(ao->id);
    free(ao);
}


/*****************************End of active object functions******************************************************************/

struct queue* Q1;//Caesar_Cipher queue
struct queue* Q2;//Change_letter_case queue
struct queue* Q3;//send_to_client queue

void* thread_handler(void *new_fd){
    int socket=*(int*) new_fd;
    pthread_detach(pthread_self());
    char buffer[2048];
    while (1){
        int msg_len=recv(socket,buffer,sizeof(buffer),0);
        printf("Recv:   %s\n",buffer);
        if(msg_len<=0){
            perror("recv");
            pthread_exit(NULL);
            break;
        }
        struct node_data* recv_msg_node=(struct node_data*)malloc(sizeof(struct node_data));
        recv_msg_node->sock=socket;
        *(buffer+msg_len) = '\0';
        //memset(recv_msg_node->data, 0, sizeof(recv_msg_node->data));
        memcpy(recv_msg_node->data,buffer, sizeof(buffer));
        enQ(Q1,recv_msg_node);
    }
    

    close((int)socket);
}

void* enQ2(void* node_after_q1){
    enQ(Q2,node_after_q1);
}

void* enQ3(void* node_after_q2){
    enQ(Q3,node_after_q2);
}

// this function dont do anything AO3 need 2 func.
void* pass_func(void* data){
    return data;
}

void* send_to_client(void* node1){
    struct node_data* send_node=(struct node_data*)node1;
    int socket=send_node->sock;
    printf("Sending:  %s  to client\n",send_node->data);
    if (send(socket, send_node->data, 2048, 0) == -1)  {
        perror("send");
    }
    free(send_node);
}

void sigchld_handler(int s)
{
    (void)s; // quiet unused variable warning

    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
    Q1 = createQ();//Create Caesar_Cipher queue
    Q2 = createQ();//Create Change_letter_case queue
    Q3 = createQ();//Create send_to_client queue
    AO* firstAO = newAO(Q1, Caesar_Cipher,enQ2);
    AO* secondAO = newAO(Q2, Change_letter_case , enQ3);
    AO* thirdAO = newAO(Q3, pass_func, send_to_client);
    int sockfd, new_fd;  // listen on sock_fd,  new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
        printf("server: got connection from %s  %d\n", s,new_fd);
        pthread_t C_thread;
        if(pthread_create(&C_thread,NULL,thread_handler,&new_fd)==0){
            printf("Thread created\n");
        }
        else{
            perror("Thread error");
            close(new_fd);
        }
    }

    return 0;
}