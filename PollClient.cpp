/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <arpa/inet.h>


#define PORT "3490" // the port client will be connecting to 

#define MAXDATASIZE 2048 // max number of bytes we can get at once 

int sockfd;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void* send_msg(void*){
    int connected = 1;
    printf("Enter you msg whenever you want,client always listening\n");
    while (connected) {
        char word[2048];
        scanf("%[^\n]%*c", word);
        if (send(sockfd, word, strlen(word), 0) == -1)  {
            perror("send");
            connected=0;
        }
        memset(word, 0, 2048);
}
    return NULL;
}

void* recv_msg(void*)   
{   
    int connected = 1;
    int bytes_recved = 0;
    char word[2048];
    while (connected)
    {
        bytes_recved = recv(sockfd, word, 1024, 0);
        if (bytes_recved<=0)
        {
            perror("recv");
            connected = 0;
            break;
        }
        printf("output from server: %s\n",word);
       memset(word, 0, 2048);
    }
    return NULL;
}


int main(int argc, char *argv[])
{
  
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo("127.0.0.1", PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    /*
        While the user is connected he can send msg to the server.
    */
    pthread_t recv_thread;//2 threads that will work on recv msg and send msg.
    pthread_t send_thread;
    pthread_create(&send_thread, NULL, send_msg, NULL);
    pthread_create(&recv_thread, NULL, recv_msg, NULL);
    pthread_join(send_thread, NULL);
    close(sockfd);

    return 0;
}