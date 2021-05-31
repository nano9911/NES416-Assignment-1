/**
 * @file server_wrappers.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief all wrapper functions are implemented here
 * @version 0.1
 * @date 2021-05-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <math.h>

#define WRAPPER_INCLUDED
#define MAX_THREAD 3

struct client_handler_thread_arguments  {
    int                     sockfd;
    struct sockaddr_storage their_addr;
    socklen_t               sin_size;
    char                    *sender_addr[NI_MAXHOST],
                            *sender_svc[NI_MAXSERV];
    struct threads          *parent;
};

struct msg_handler_thread_arguments {
    long num, from, to, *rv;
};

struct threads   {
    struct threads                  *before;
    pthread_t                       tid;        /*thread id*/
    struct threads                  *next;
};

unsigned int running_threads = 0;
struct threads *first, *last;
int listenfd;

void signal_handler(int sig_no) {
    if (sig_no == SIGINT)   {
        printf("Exiting with out waiting threads -> ");
        printf("Interrupte signal arrived: SIGINT(%d)\n", sig_no);

        close(listenfd);

        /*stop all running threads which will start there cleanup processures*/
        for (struct threads *ptr = first; ptr != NULL; ptr = ptr->next) {
            pthread_cancel(ptr->tid);
            printf("Thread %ld terminated\n", ptr->tid);
        }
        exit(EXIT_SUCCESS);
    }
}

int Recv(int tcpclientfd,
        char *recv_buf, size_t buf_len,
        int flags,
        const char *sender_addr, const char *sender_svc) {

    int received = recv(tcpclientfd, recv_buf, buf_len, flags);

    if (received <= 0) {
        if (received == 0)
            printf("%s:%s closed the connection\n",
                            sender_addr, sender_svc);
        else
            perror("tcp_client_handler: recv");
        return -1;
    }

    printf("TCP Socket: Message received from %s:%s: \
(length = %ld)\n%s\n\n",
            sender_addr, sender_svc, strlen(recv_buf), recv_buf);

    return received;
}


int Send(int tcpclientfd,
        const char *send_buf, size_t buf_len,
        int flags,
        char *sender_addr, char *sender_svc)   {

    int rv = send(tcpclientfd, send_buf, buf_len, flags);
    if (rv == -1)   {
        perror("tcp_client_handler: send");
        return -1;
    }

    printf("TCP Socket: Message Sent to client %s:%s: \
(length = %ld)\n%s\n\n",
            sender_addr, sender_svc, strlen(send_buf), send_buf);

    return 0;
}
