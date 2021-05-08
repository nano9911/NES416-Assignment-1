/**
 * @file server_wrappers.h
 * @author Adnan Hleihel (JUST ID: 123423)
 * @brief Server functions wrappers for NES416/HW4
 * @version 0.1
 * @date 2021-05-08
 * 
 * @copyright Copyright (c) 2021
 * 
 */

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
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <wait.h>

#define MAX(x,y) ((x) > (y) ? (x) : (y))

#define RECV_BUF_LEN 512        // Receive buffer size in bytes
#define SEND_BUF_LEN 512        // Send buffer size in bytes

char *choices[] = {
    "Error",
    "Decryption",
    "Encryption",
    "Exit",
    "List dir. files"
};

char *type[]    = {
    "",
    "Encrypted text",
    "Decrypted text",
    "Directory files"
};

int childcount = 0;
int tcpsockfd;          /* TCP Server listening socket */
int udpsockfd;          /* UDP Server listening socket */
int tcpclientfd;        /* TCP Client socket */

/*  Will be filled with the address of the peer connection
*   (client ip and port). */
struct sockaddr_storage their_addr;
/*  Will contain the length of the returned address from 
*   accept() system call. */
socklen_t sin_size = sizeof(their_addr);

/* Will contain the IP and Port in host byte order of client
in their_addr */
char sender_addr[NI_MAXHOST], sender_svc[NI_MAXSERV];

int Recv(char *recv_buf, size_t buf_len, int flags,
        char *sender_addr, char *sender_svc) {

    int received = recv(tcpclientfd, recv_buf, buf_len, flags);

    if (received <= 0) {
        if (received == 0)
            printf("%s:%s closed the connection\n",
                            sender_addr, sender_svc);

        else    {
            perror("tcp_client_handler: recv");
            close(tcpclientfd);
            close(udpsockfd);
            exit(EXIT_FAILURE);
        }
    }

    printf("Received message from %s:%s:\t%s\n",
            sender_addr, sender_svc, recv_buf);
    return received;
}

void Send(const char *send_buf, size_t buf_len, int flags,
        char *sender_addr, char *sender_svc)   {

    int rv = send(tcpclientfd, send_buf, buf_len, flags);
    if (rv == -1)   {
        rv = EXIT_FAILURE;
        perror("tcp_client_handler: send");
        close(tcpclientfd);
        close(udpsockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent \"%s\" of length %ld to the client %s:%s\n\n",
            send_buf, buf_len, sender_addr, sender_svc);
}

int Recvfrom(char *__restrict__ recv_buf, size_t buf_len,
            int flags, struct sockaddr_storage *__restrict__ their_addr,
            socklen_t *__restrict__ sin_size)    {

    int received = recvfrom(udpsockfd, recv_buf, buf_len, flags,
            (struct sockaddr *)their_addr, sin_size);
    if (received == -1) {
        perror("udp_conn: recv");
        close(tcpsockfd);
        close(udpsockfd);
        exit(EXIT_FAILURE);
    }

    return received;
}

void Sendto(const char *send_buf, ssize_t buf_len, int flags,
            const struct sockaddr_storage *their_addr, socklen_t sin_size,
            char *sender_addr, char *sender_svc)   {

    ssize_t rv = sendto(udpsockfd, send_buf, buf_len+1, flags,
        (struct sockaddr *)their_addr, sin_size);

    if (rv != buf_len+1)   {
        perror("udp_conn: send");
        close(tcpsockfd);
        close(udpsockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent \"%s\" to client %s:%s\n\n",
            send_buf, sender_addr, sender_svc);
}

int Accept(struct sockaddr_storage *their_addr,
            socklen_t *__restrict__ sin_size)   {
    /*  accept() will return the client socket to clientfd.
    *   Also, it will fill their_addr with the IP address and
    *   port number of the peer of the connecion (client) in
    *   network byte order, and the length of it in sin_size. */
    int tcpclientfd = accept(tcpsockfd,
                        (struct sockaddr *)their_addr,
                        sin_size);

    if (tcpclientfd == -1)     {
        perror("tcp_conn: accept");
        close(tcpsockfd);
        close(udpsockfd);
        exit(EXIT_FAILURE);
    }

    return tcpclientfd;
}

void Signal(int __sig, sig_t __handler) {
    sig_t sig = signal(__sig, __handler);
    if (sig == SIG_ERR)    {
        perror("server: signal");
        close(tcpsockfd);
        close(udpsockfd);
        exit(EXIT_FAILURE);
    }
}