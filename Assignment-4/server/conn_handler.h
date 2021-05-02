/**
 * @file conn_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Connection Handler Header file for NES416/HW4
 * @date 2021-05-01
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "message_handler.h"
#include "socket_handler.h"

#define RECV_BUF_LEN 512        // Receive buffer size in bytes
#define SEND_BUF_LEN 512        // Send buffer size in bytes
#define MAX_TEXT_LEN 128

char *choices[] = {"Encrypt", "Decrypt", "Exit", "List"};

int tcpclientfd;               /* TCP Client handler socket */
int childcount = 0;

/* Will be filled with the address of the peer connection (client ip and port). */
struct sockaddr_storage their_addr;
/* Will contain the length of the returned address from aacept() syetem call. */
socklen_t sin_size = sizeof(their_addr);

/* Will contain the IP and Port in host byte order of client
in their_addr */
char sender_addr[NI_MAXHOST], sender_svc[NI_MAXSERV];

int tcp_client_handler();

static int udp_conn(int sockfd)    {
    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int sent=0, received=0;

    memset(&their_addr, 0, sizeof(their_addr));
    sin_size = sizeof their_addr;

    received = recvfrom(sockfd, recv_buf, RECV_BUF_LEN, 0,
                (struct sockaddr *)&their_addr, sin_size);

    if (received == -1) {
        perror("udp_conn: recvfrom");
        return -1;
    }

    /*  getnameinfo() will convert the IP:Port from network byte
    *   order to host byte order.
    *   The address will be saved as string in host byte order in
    *   peer_name and peer_port.*/
    getnameinfo((struct sockaddr *)&their_addr, sin_size,
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);
    printf("server: got connection from %s:%s\n", sender_addr, sender_svc);

    return 0;
}


static int tcp_conn(int sockfd)  {
    int rv=0, pid;
    memset(&their_addr, 0, sizeof(their_addr));
    sin_size = sizeof their_addr;

    /*  accept() will return the client socket to clientfd.
    *   Also, it will fill their_addr with the IP address and
    *   port number of the peer of the connecion (client) in
    *   network byte order, and the length of it in sin_size. */
    tcpclientfd = accept(sockfd,
                        (struct sockaddr *)&their_addr,
                        &sin_size);

    if (tcpclientfd == -1)     {
        perror("tcp_conn: accept");
        return -1;
    }

    /*  Creating a child process to handle the new accepted
    *   connection, and the parent will return to accept more
    *   new clients. */
    pid = fork();
    if (pid == 0)   {
        /*  child don't need that*/
        close(sockfd);
        rv = tcp_client_handler(tcpclientfd);
        close(tcpclientfd);
        exit(rv);
    }

    childcount++;
    /*  parent doesn't need that    */
    if (close(tcpclientfd)) return -1;
    return 0;
}

/*****************************************************************/
/**
 * @brief child proccess function to handle tcp connection after
 * fork() in parent. From the start of this file we made the
 * required parameters global (out of main()), to get rid of any
 * parameters passing issues.
 * 
 * @return int, defines the state or the cause of the connection
 * termination process.
 */
int tcp_client_handler(int sockfd)
{
    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN], msg[MAX_TEXT_LEN];
    int rv, received = 0, msglen=0;

    /*  choice is the first character from the client message   */
    int choice = ERR;

    /*  getnameinfo() will convert the IP:Port from network byte
    *   order to host byte order.
    *   The address will be saved as string in host byte order in
    *   peer_name and peer_port.*/
    getnameinfo((struct sockaddr *)&their_addr, sin_size,
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);
    printf("server: got connection from %s:%s\n", sender_addr, sender_svc);

    while (1) {
        /* We zeroed (reseted) recv_buf, to clean it for the next data coming. */
        memset(recv_buf, 0, RECV_BUF_LEN);

        printf("waiting for client: %s:%s messages...\n", sender_addr, sender_svc);
        received = recv(sockfd, recv_buf, RECV_BUF_LEN, 0);
        if (received <= 0) {
            perror("tcp_client_handler: recv");
            break;
        }

        rv = decode_msg(recv_buf, received, msg, &msglen);

        if (rv == -1)   {sprintf(send_buf, "Invalid choice.");}
        if (rv == ENC || rv == DEC) {
            rv = cipher(msg, msglen, rv);
        }

        rv = send(sockfd, send_buf, strlen(send_buf), 0);
        printf("Sending \"%s\" to the client\n\n", send_buf);
        if (rv == -1)   {
            perror("tcp_client_handler: send");
            break;
        }
    }

    printf("Closing connection with %s:%s\n", sender_addr, sender_svc);
    return rv;
}

