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
#define SEND_BUF_LEN 1024       // Send buffer size in bytes
#define QUIT 99                 // signal from child when ended to end server

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

void wait_children_and_exit(int rv)   {
    int chrv=0;

    /*  if parent ended unexpectedly, we should wait for live children to end    */
    for (int i = 0; i < childcount; i++)    {
        int pid = waitpid(-1, &chrv, WNOHANG);
        printf("%d child process end with exit code %d\n", pid, chrv);
        childcount--;
    }

    close(tcpsockfd);
    close(udpsockfd);
    printf("Parent Server Going down after closed all sockets\nFinished\n");
    exit(rv);
}

int Recv(char *recv_buf, size_t buf_len, int flags,
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
    
//    else if (recv_buf[0] == '3' && recv_buf[1] == '-' && recv_buf[2] == '-' && strlen(recv_buf) == 3) {
    else if (strncmp(recv_buf, "3--", 3) == 0)   {
        printf("TCP Socket: Closing program: Client %s:%s sent Termination signal \"3--\"\n",
                sender_addr, sender_svc);

        close(tcpclientfd);
        exit(QUIT);
    }

    printf("/*****************************************************************/\n");
    printf("TCP Socket: Message received from %s:%s: (length = %ld)\n%s\n",
            sender_addr, sender_svc, strlen(recv_buf), recv_buf);
    printf("/*****************************************************************/\n\n");

    return received;
}

void Send(const char *send_buf, size_t buf_len, int flags,
        char *sender_addr, char *sender_svc)   {

    int rv = send(tcpclientfd, send_buf, buf_len, flags);
    if (rv == -1)   {
        rv = EXIT_FAILURE;
        perror("tcp_client_handler: send");
        close(tcpclientfd);
        exit(EXIT_FAILURE);
    }

    printf("/*****************************************************************/\n");
    printf("TCP Socket: Message Sent to client %s:%s: (length = %ld)\n%s\n",
            sender_addr, sender_svc, strlen(send_buf), send_buf);
    printf("/*****************************************************************/\n\n");
}

int Recvfrom(char *__restrict__ recv_buf, size_t buf_len,
            int flags, struct sockaddr_storage *__restrict__ their_addr,
            socklen_t *__restrict__ sin_size)    {

    int received = recvfrom(udpsockfd, recv_buf, buf_len, flags,
            (struct sockaddr *)their_addr, sin_size);
    if (received == -1) {
        perror("udp_conn: recv");

        wait_children_and_exit(EXIT_FAILURE);
    }

    else if (strlen(recv_buf) == 0)
        return 0;
    
    else if (strncmp(recv_buf, "3--", 3) == 0)   {
        printf("UDP Socket: Closing server program: Client %s:%s user sent Termination signal\n",
                sender_addr, sender_svc);
        
        wait_children_and_exit(EXIT_SUCCESS);
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
        wait_children_and_exit(EXIT_FAILURE);
    }

    printf("/*****************************************************************/\n");
    printf("UDP Socket: Message Sent to client %s:%s: (length = %ld)\n%s\n",
            sender_addr, sender_svc, strlen(send_buf), send_buf);
    printf("/*****************************************************************/\n\n");
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
        wait_children_and_exit(EXIT_FAILURE);
    }

    return tcpclientfd;
}
