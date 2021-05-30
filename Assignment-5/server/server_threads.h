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

extern int test_primality(long num, time_t *t);

/**
 * @brief message handler thread, which will check the primality of a given
 * number to a range of number
 * 
 * @param argv 
 * @return void* 
 */
void *msg_thread(void *argv)    {
    struct msg_thread_arguments *args = (struct msg_thread_arguments *)argv;
    long rv = -1;

    for (long i = args->from; i <= args->to; i++)    {
        if (args->num % i == 0) {
            rv = i;
            break;
        }
    }

    free(args);
    pthread_exit((void *)&rv);
}

/**
 * @brief client handler thread, starts after connection accepted
 * 
 * @param argv 
 * @return void* 
 */
void *client_thread(void *argv)   {
    /*  get calling parent thread passed argument to local thread   */
    struct client_thread_arguments *args =
        (struct client_thread_arguments *)argv;

    /*  get current thread id   */
    pthread_t   threadid = pthread_self();
    long        num;
    char        sender_addr[15], sender_svc[5],
                recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int         rv;
    time_t      sec;

    /*  getnameinfo() will convert the IP:Port from network byte
    *   order to host byte order.
    *   The address will be saved as string in host byte order in
    *   peer_name and peer_port.
    */
    getnameinfo((struct sockaddr *)&(args->their_addr), args->sin_size,
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);

    printf("New thread with ID %ld started to handle client %s:%s\n",
                    threadid, sender_addr, sender_svc);

    rv = Recv(args->sockfd, recv_buf, RECV_BUF_LEN, 0,
                                sender_addr, sender_svc);

    if (rv > 0) {
        num = strtol(recv_buf, NULL, 10);

        if (errno != EINVAL && errno != ERANGE && rv > 0) {
            if (test_primality(num, &sec) == 1)
                sprintf(send_buf,"Prime\tProcess took %ld seconds", sec);
            else
                sprintf(send_buf,"Not Prime\tProcess took %ld seconds", sec);
        }

        rv = Send(args->sockfd, send_buf, strlen(send_buf),
                    0, sender_addr, sender_svc);
    }

    close(args->sockfd);
    free(args);
    pthread_exit(0);
}
