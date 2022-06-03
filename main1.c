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

#include "queue.h"
#include "active_object.h"

#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10	 // how many pending connections queue will hold


struct queue* Q1;//Caesar_Cipher queue
struct queue* Q2;//Change_letter_case queue
struct queue* Q3;//send_to_client queue

/**
 * @brief after a user is connected this fucntion receive data from the user and enter the data to Q1.
 * 
 * @param new_fd
 * @return void* 
 */
void* Client_thread_handler(void *new_fd){
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
        memcpy(recv_msg_node->data,buffer, sizeof(buffer));
        enQ(Q1,recv_msg_node);
    }
    

    close((int)socket);
}

/**
 * @brief Ceaser cipher function with a left shift of 1.
 * @param word - the word to cipher
 * @return void* to the void* queue
 */
void* Caesar_Cipher(void* word){
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

/**
 * @brief receive a void* node from Q1 and pass him to Q2
 * 
 * @param node_after_q1 
 */
void* enQ2(void* node_after_q1){
    enQ(Q2,node_after_q1);
}

/**
 * @brief Change the letter case from upper to lower or backwards 
 * @param word the word we receive from Q2
 * @return void* to the void* queue
 */
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

/**
 * @brief receive a void* node from Q2 and pass him to Q3
 * 
 * @param node_after_q2 
 */
void* enQ3(void* node_after_q2){
    enQ(Q3,node_after_q2);
}

// this function dont do anything just pass the data the the next function the the AO.
void* pass_func(void* data){
    return data;
}

/**
 * @brief after the server received a word from the client and the word passed a Caesar_Cipher and change the letter case of the word this function send the new word back to the client.
 * 
 * @param node1 
 * @return void* 
 */
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
        if(pthread_create(&C_thread,NULL,Client_thread_handler,&new_fd)==0){
            printf("Thread created\n");
        }
        else{
            perror("Thread error");
            close(new_fd);
        }
    }

    return 0;
}