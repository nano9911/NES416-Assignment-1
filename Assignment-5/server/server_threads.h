/**
 * @file server_threads.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief server threads implementation
 * @version 0.1
 * @date 2021-05-30
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#ifndef WRAPPER_INCLUDED
#include "server_wrappers.h"
#endif /*ifndef WRAPPER_INCLUDED*/

#define RECV_BUF_LEN 16
#define SEND_BUF_LEN 64
#define THREADS_DEFINED


void client_handler_thread_cleanup(void *argv)    {
    struct client_handler_thread_arguments *args =
        (struct client_handler_thread_arguments *)argv;

    close(args->sockfd);

    if (args->parent != first && args->parent != last)    {

        args->parent->before->next = args->parent->next;
        args->parent->next->before = args->parent->before;
    }

    else    {
        /*if there's only one entry*/
        if (first == last)  {
            first = NULL;
            last = NULL;
        }
        else if (args->parent == first) {
            first = args->parent->next;
            args->parent->next->before = NULL;
        }
        else if (args->parent == last)  {
            last = args->parent->before;
            args->parent->before->next = NULL;
        }
    }

    free(args->parent);
    free(args);
    running_threads--;

//    return NULL;
}

/**
 * @brief client handler thread, starts after connection accepted
 * 
 * @param argv 
 * @return void* 
 */
void *client_handler_thread(void *argv)   {
    /**
     * @brief get calling parent thread passed argument to local thread*/
    struct client_handler_thread_arguments *args =
        (struct client_handler_thread_arguments *)argv;

    /*  get current thread id   */
    pthread_t   threadid = pthread_self();
    long        num;
    char        sender_addr[NI_MAXHOST], sender_svc[NI_MAXSERV],
                recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int         rv;
    time_t      sec;
    pthread_t   msgid[MAX_THREAD];

    /**
     * @brief getnameinfo() will convert the IP:Port from network byte
     * order to host byte order.
     * The address will be saved as string in host byte order in
     * peer_name and peer_port. */
    getnameinfo((struct sockaddr *)&(args->their_addr),
                args->sin_size,
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);

    printf("New thread with ID %ld started to handle client %s:%s\n",
                    threadid, sender_addr, sender_svc);

    /*prepare cleanup function*/
    pthread_cleanup_push(client_handler_thread_cleanup, args);

    rv = Recv(args->sockfd, recv_buf, RECV_BUF_LEN, 0,
                                sender_addr, sender_svc);

    if (rv > 0) {
        num = strtol(recv_buf, NULL, 10);

        if (errno != EINVAL && errno != ERANGE && rv > 0) {
            if (test_primality(num, &sec, msgid) == 1)
                sprintf(send_buf,"Prime\tProcess took %ld seconds", sec);
            else
                sprintf(send_buf,"Not Prime\tProcess took %ld seconds", sec);
        }

        rv = Send(args->sockfd, send_buf, strlen(send_buf),
                    0, sender_addr, sender_svc);
    }

    pthread_cleanup_pop(1);
    return NULL;
}
