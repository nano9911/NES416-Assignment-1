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

#ifndef WRAPPER_INCLUDED
#include "server_wrappers.h"
#endif /*ifndef WRAPPER_INCLUDED*/

#include "server_sock_handler.h"
#include "server_msg_handler.h"
#include "server_threads.h"

void add_thread(struct threads *temp);

int main(int argc, char *argv[])    {
    if (argc < 2)   {
        printf("Usage: %s [port number]", argv[0]);
        exit(0);
    }

    struct threads  *temp=NULL;
    /**
     * @brief args is a pointer to arguments passed to the newly
     * created thread  */
    struct client_handler_thread_arguments *args;
    pthread_attr_t  attr;
    int             listenfd=get_tcp_socket(NULL, argv[1]),
                    newsockfd;
    char            sender_addr[NI_MAXHOST],
                    sender_svc[NI_MAXSERV];

    /*create a deteched attribute to the newly created threads*/
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    while(1) {
        printf("Waiting for connection...\n\n");
        newsockfd = accept( listenfd,
                            (struct sockaddr *)&(args->their_addr),
                            &(args->sin_size));
        if (newsockfd == -1)    {
            perror("main: accept");
            break;
        }

        getnameinfo((struct sockaddr *)&(args->their_addr),
                    args->sin_size,
                    sender_addr, sizeof(sender_addr),
                    sender_svc, sizeof(sender_svc),
                    NI_NUMERICHOST | NI_NUMERICSERV);
        printf("Accepted connection from %s:%s\n",
                sender_addr, sender_svc);

        /**
         * @brief prepare thread id and arguments structure,
         * and initialize it's objects */
        temp = (struct threads *)
                malloc(sizeof(struct threads));
        args = (struct client_handler_thread_arguments *)
                malloc(sizeof(struct client_handler_thread_arguments));

        args->sin_size = sizeof args->their_addr;
        args->parent = temp;

        if (pthread_create(&(temp->tid), &attr,
                            client_handler_thread, (void *)args) != 0)  {
            perror("main: pthread_create");
            break;
        }

        add_thread(temp);
    }

    close(listenfd);

    for (struct threads *ptr = first; ptr != NULL; ptr = ptr->next)
        printf("Thread %ld finished, and handled correctly\n\n", ptr->tid);

    exit(0);
}

/**
 * @brief add new thread id to threads id linked list
 * 
 * @param temp 
 */
void add_thread(struct threads *temp)   {
    running_threads++;

    if (first == NULL && last == NULL)  {
        first = temp;
        last = first;
        first->before = NULL;
        last->next = NULL;
    }

    else    {
        last->next = temp;
        temp->before = last;
        last = temp;
        last->next = NULL;
    }
}
