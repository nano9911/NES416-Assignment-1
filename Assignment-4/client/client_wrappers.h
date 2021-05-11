/**
 * @file client_wrappers.h
 * @author Adnan Hleihel (JUST ID: 123423)
 * @brief Client functions wrapper for NES416/HW4
 * @version 0.1
 * @date 2021-05-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define RECV_BUF_LEN 1024        // Receive buffer size in bytes
#define SEND_BUF_LEN 256        // Send buffer size in bytes

int sockfd;
char peer_name[30], peer_port[10];

char *menu[] = {
    "1. Encrypt a message,",
    "2. Decrypt a message.",
    "3. Exit.",
    "4. Listing files in the current directory on the server."
};

char *type[]    = {
    "",
    "Encrypted text",
    "Decrypted text",
};

int check_input_and_return_choice(int argc, char *argv[])
{
    if (argc != 4)  {
        printf("Usage: ./%s [server name] [service name] [udp or tcp]\n",
                argv[0]);
        exit(EXIT_SUCCESS);
    }

    int choice;
    /*  check user choice from cli arguments
     *  and assign it if it's valid */
    if (strncmp("udp", argv[3], 3) != 0 &&
        strncmp("tcp", argv[3], 3) != 0) {

        fprintf(stderr, "Third argument should be either \
\"tcp\" or \"udp\"\n");
        exit(EXIT_SUCCESS);
    }

    choice = ((strncmp("tcp", argv[3], 3) == 0) ? 1 : 0);

    if (strncmp("tcp", argv[3], 3) == 0)    {
        printf("TCP Connection, choice = %d\n", choice);
    }

    if (strncmp("udp", argv[3], 3) == 0)    {
        printf("UDP Connection, choice = %d\n", choice);
    }

    return choice;
}

void Send(const char *send_buf, ssize_t buf_len, int flags)   {
    int sent = send(sockfd, send_buf, buf_len+1, flags);

    if (sent != buf_len+1)    {
        perror("send failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
}

void Recv(char *recv_buf, size_t buf_len, int choice)   {
    int received = read(sockfd, recv_buf, buf_len);
    if (received <= 0) {
        if (received < 0)   {
            perror("read failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        else if (received == 0 && choice == 1)   {
            printf("TCP Connection closed: FIN received.\n");
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
    }
}
