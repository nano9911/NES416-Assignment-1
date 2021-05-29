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
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

int Recv(int tcpclientfd, char *recv_buf, size_t buf_len, int flags,
        char *sender_addr, char *sender_svc) {

    int received = recv(tcpclientfd, recv_buf, buf_len, flags);

    if (received <= 0) {
        if (received == 0)
            printf("%s:%s closed the connection\n",
                            sender_addr, sender_svc);

        else
            perror("tcp_client_handler: recv");

        close(tcpclientfd);
        exit(EXIT_FAILURE);
    }

    else if (strlen(recv_buf) == 0) {
        printf("TCP Socket: Empty Message received from %s:%s and ignored\n\n",
            sender_addr, sender_svc);
        return 0;
    }

    printf("/*****************************************************************/\n");
    printf("TCP Socket: Message received from %s:%s: (length = %ld)\n%s\n",
            sender_addr, sender_svc, strlen(recv_buf), recv_buf);
    printf("/*****************************************************************/\n\n");

    return received;
}
