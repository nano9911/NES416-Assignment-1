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

#include "server_sock_handler.h"
#include"server_conn_handler.h"

struct threads   {
    struct threads *before;
    pthread_t tid;
    struct threads *next;
    int available;
};

void add_thread(struct threads *first,
                struct threads *last,
                pthread_t newid)   {

    struct threads *temp;
    if (first->available == 0 && last->available == 0)  {
        first = (struct threads *)malloc(sizeof(struct threads));

        first->tid = newid; first->available = 1;
        last = first;
    }

    else if (first->available == 1 && last->available == 1) {
        temp = (struct threads *)malloc(sizeof(struct threads));

        last->next = temp;
        temp->before = last;
        last = temp;
        last->tid = newid;
        last->available = 1;
    }
}

int main(int argc, char *argv[])    {
    if (argc < 2)   {
        printf("Usage: %s [port number]", argv[0]);
        exit(0);
    }

    struct threads *first=NULL, *last=first;
    pthread_t temp;
    int sockfd = get_tcp_socket(NULL, argv[1]),
        newsockfd;

    while(1) {
        struct client_thread_arguments *args =
        (struct client_thread_arguments *)malloc(sizeof(struct client_thread_arguments));
        args->sin_size = sizeof args->their_addr;

        newsockfd = accept( sockfd,
                            (struct sockaddr *)&(args->their_addr),
                            &(args->sin_size));
        if (newsockfd == -1)    {
            perror("main: accept");
            break;
        }

        if (pthread_create(&temp, NULL, client_thread, (void *)args) != 0)  {
            perror("main: pthread_create");
            break;
        }
        add_thread(first, last, temp);
    }

    exit(0);
}
