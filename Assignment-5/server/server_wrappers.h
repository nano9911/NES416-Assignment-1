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

//#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <math.h>

#define WRAPPER_INCLUDED

struct client_thread_arguments  {
    int                     sockfd;
    struct sockaddr_storage their_addr;
    socklen_t               sin_size;
};

struct msg_thread_arguments {
    long num, from, to;
};

struct threads   {
    struct threads *before;
    pthread_t tid;
    struct threads *next;
    int available;
};


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

    else if (strlen(recv_buf) == 0) {
        printf("TCP Socket: Empty Message received from %s:%s, \
closing connection\n\n",
            sender_addr, sender_svc);
        return 0;
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
