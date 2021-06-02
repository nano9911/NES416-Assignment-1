/**
 * @file client_wrappers.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief all wrapper functions are implemented here
 * @version 0.1
 * @date 2021-06-01
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
//#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>

#define WRAPPER_INCLUDED
#define SEND_BUF_LEN    16
#define RECV_BUF_LEN    128

struct thread_arguments {
    char    *target_addr,
            *target_svc;
    long    num;
    int id;
};

struct thread_cleanup_arguments {
    pthread_t tid;
    int id;
    int sockfd;
};

pthread_t   t1, t2;
//char        server_addr[NI_MAXHOST],
//            svc_number[NI_MAXSERV];

void signal_handler(int sig_no) {
    if (sig_no == SIGINT)   {
        printf("Exiting with out waiting threads -> ");
        printf("Interrupte signal arrived: SIGINT(%d)\n", sig_no);

        /*stop all running threads which will start there cleanup processures*/
        pthread_cancel(t1);
        printf("Thread  1 (%ld) terminated\n", t1);
        pthread_cancel(t2);
        printf("Thread 2 (%ld) terminated\n", t2);

        exit(EXIT_SUCCESS);
    }
}

int Send(int sockfd, const char *send_buf, ssize_t buf_len, int flags)   {
    int sent = send(sockfd, send_buf, buf_len, flags);

    if (sent != buf_len)    {
        perror("send failed");
        return -1;
    }
    return 0;
}

int Recv(int sockfd, char *recv_buf, size_t buf_len, int flags)   {
    int received = recv(sockfd, recv_buf, buf_len, flags);

    if (received <= 0) {
        if (received < 0)   {
            perror("read failed");
        }
        else if (received == 0)   {
            printf("TCP Connection closed: FIN received.\n");
        }
        return -1;
    }

    return 0;
}
