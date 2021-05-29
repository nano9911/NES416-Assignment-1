/**
 * @file server.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief server main file
 * @version 0.1
 * @date 2021-05-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include"server_wrappers.h"
#include "server_conn_handler.h"

#define MAX_THREADS 5

struct client_thread_arguments  {
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
};

struct msg_thread_arguments {
    long num, from, to;
};

void *client_thread(void *argv);
void *msg_thread(void *argv);

int main(int argc, char *argv[])    {
    if (argc < 2)   {
        printf("Usage: %s [port number]", argv[0]);
        exit(0);
    }

    pthread_t *thid;
    int sockfd = get_tcp_socket(NULL, argv[1]),
        newsockfd;

    while(1) {
        struct client_thread_arguments args;
        args.sin_size = sizeof args.their_addr;
        
        newsockfd = accept(sockfd,(struct sockaddr *)&(args.their_addr), &(args.sin_size));
        if (newsockfd == -1)    {
            perror("main: accept");
            break;
        }

        pthread_create(thid, NULL, client_thread, &args);
    }

    exit(0);
}

/**
 * @brief client handler thread, starts after connection accepted
 * 
 * @param argv 
 * @return void* 
 */
void *client_thread(void *argv)   {
    struct client_thread_arguments *args = (struct client_thread_arguments *)argv;
    char sender_addr[15], sender_svc[5];

    /*  getnameinfo() will convert the IP:Port from network byte
    *   order to host byte order.
    *   The address will be saved as string in host byte order in
    *   peer_name and peer_port.
    */
    getnameinfo((struct sockaddr *)&(args->their_addr), (args->sin_size),
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);

    pthread_t threadid = pthread_self();



    pthread_exit(0);
}

void *msg_thread(void *argv)    {
    struct msg_thread_arguments *args = (struct msg_thread_arguments *)argv;
    int rv = 0;

    pthread_exit(rv);
}
