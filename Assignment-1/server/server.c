/**
 * @file HW1_server.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Main server file of NES416/HW1
 * @date 2021-03-17
 *
 */


#include "socket_handler.h"
#include "message_handler.h"

#define RECV_BUF_LEN 128        // Receive buffer size in bytes
#define SEND_BUF_LEN 20         // Send buffer size

int main(int argc, char *argv[])
{
    if (argc != 2)  {
        printf("Usage: ./calc_server [port no.]\n");
        exit(1);
    }

    int sockfd,               /* Server listening socket */
        tcpclientfd;               /* Client handler socket */

    /*  hints -> We will fill it with out server specifications
    *   (ip version, transport protocol, some flags). */
    struct addrinfo hints;

    /* Will be filled with the address of the peer connection (client ip and port). */
    struct sockaddr_storage their_addr;

    /* Will contain the length of the returned address from aacept() syetem call. */
    socklen_t sin_size = sizeof(their_addr);

    /* Will contain the IP and Port in host byte order of client in their_addr */
    char sender_addr[NI_MAXHOST], sender_svc[NI_MAXSERV];

    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];

    int rv, received = 0, result;

    /*  Firstly, we will clear (zeroing) the hints struct (for safety). Then,
    *   we will fill it with our configuration of the server.
    *   AF_INET -> IPv4     |     SOCK_STREAM -> TCP
    *   AI_PASSIVE -> We will use our this machine ip. */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    /* argv[1] is the port number you want the server to bind and listen to. */
    CreateSocket(NULL, argv[1], &hints, &sockfd);

    while(1)    {
        printf("\n\nwaiting for connections...\n\n");
        memset(&their_addr, 0, sizeof(their_addr));

        /*  accept() will return the client socket to clientfd.
        *   Also, it will fill their_addr with the IP address and port number of
        *   the peer of the connecion (client) in network byte order, and the
        *   length of it in sin_size. */
        tcpclientfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
        if (tcpclientfd == -1)     {
            perror("accept");
            break;
        }

        memset(sender_addr,0,sizeof(sender_addr));
        memset(sender_svc,0,sizeof(sender_svc));
        /*  getnameinfo() will convert the IP:Port from network byte order to
        *   host byte order.
        *   The address will be saved as string in host byte order in peer_name
        *   and peer_port.*/
        getnameinfo((struct sockaddr *)&their_addr, sin_size, sender_addr, sizeof(sender_addr),
                    sender_svc, sizeof(sender_svc), NI_NUMERICHOST | NI_NUMERICSERV);
        printf("server: got connection from %s:%s\n", sender_addr, sender_svc);

        while (1) {
            /* We zeroed (reseted) recv_buf, to clean it for the next data coming. */
            memset(recv_buf, 0, RECV_BUF_LEN);
            printf("waiting for client: %s:%s messages...\n", sender_addr, sender_svc);
            received = recv(tcpclientfd, recv_buf, RECV_BUF_LEN, 0);
            if (received <= 0) {
                perror("server: recv");
                close(tcpclientfd);
                break;
            }

            printf("\nreceived \"%s\" from the client %s:%s\n", recv_buf, sender_addr, sender_svc);

            if (strcmp(recv_buf, "exit") == 0)   {
                printf("exiting\n");
                close(tcpclientfd);
                break;
            }

            /* Pass our handler the message. */
            rv = handle_msg(recv_buf, received, &result);

            if (rv == -1)   {sprintf(send_buf, "Invalid values.");}
            else if (rv == -2)   {sprintf(send_buf, "Invalid opration.");}
            else if (rv == -2)   {sprintf(send_buf, "Division by zero.");}
            else if (rv == 0)    {sprintf(send_buf, "%d", result);}

            rv = send(tcpclientfd, send_buf, strlen(send_buf), 0);
            printf("Sending \"%s\" to the client\n\n", send_buf);
            if (rv == -1)   {
                perror("server: send");
                close(tcpclientfd);
                break;
            }
        }

        printf("Closing connection with %s:%s\n", sender_addr, sender_svc);
        close(tcpclientfd);  /* We don't need this, so close it.. */
    }

    close(sockfd);
    exit(0);
}
