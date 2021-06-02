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

#define RECV_BUF_LEN 32
#define SEND_BUF_LEN 128
#define THREADS_DEFINED


void client_handler_thread_cleanup(void *argv)    {
    struct client_handler_thread_arguments *args =
        (struct client_handler_thread_arguments *)argv;

    close(args->sockfd);

    printf("Thread %ld terminated and connection with client %s:%s is closed properly\n\n",
        args->parent->tid, args->sender_addr, args->sender_svc);

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
    char        recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    long         rv;
    pthread_t   msgid[MAX_THREAD];
    struct timeval  start, stop;
    double secs=0;

    /**
     * @brief getnameinfo() will convert the IP:Port from network byte
     * order to host byte order.
     * The address will be saved as string in host byte order in
     * peer_name and peer_port. */
    getnameinfo((struct sockaddr *)&(args->their_addr),
                args->sin_size,
                args->sender_addr, sizeof(args->sender_addr),
                args->sender_svc, sizeof(args->sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);

    printf("New thread with ID %ld started to handle client %s:%s\n",
                    threadid, args->sender_addr, args->sender_svc);

    /*prepare cleanup function*/
    pthread_cleanup_push(client_handler_thread_cleanup, args);

    rv = Recv(args->sockfd, recv_buf, RECV_BUF_LEN, 0,
                    args->sender_addr, args->sender_svc);

    if (rv > 0) {
        rv = -1;
        num = atol(recv_buf);
        gettimeofday(&start, NULL);

        rv = test_primality(num, msgid);

        gettimeofday(&stop, NULL);
        secs = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);

        if (rv == -1)
            sprintf(send_buf,"%ld is Prime (Time: %f seconds)", num, secs);
        else
            sprintf(send_buf,"%ld is not Prime because %ld is a factor of it (Time: %f seconds)", num, rv, secs);

        rv = Send(args->sockfd, send_buf, strlen(send_buf),
                    0, args->sender_addr, args->sender_svc);
    }
    pthread_exit(NULL);
    pthread_cleanup_pop(1);
}
