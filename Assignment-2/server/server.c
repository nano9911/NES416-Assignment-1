/**
 * @file HW1_server.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Main server file of NES416/HW2
 * @date 2021-03-17
 *
 */


#include "socket_handler.h"
#include "message_handler.h"
#include <signal.h>

#define RECV_BUF_LEN 128        // Receive buffer size in bytes
#define SEND_BUF_LEN 20         // Send buffer size in bytes

int listenfd,               /* Server listening socket */
    clientfd;               /* Client handler socket */

int childcount = 0;

/* Will be filled with the address of the peer connection (client ip and port). */
struct sockaddr_storage their_addr;

/* Will contain the length of the returned address from aacept() syetem call. */
socklen_t sin_size = sizeof(their_addr);

int client_handler();

void signal_handler(int sig_no)   {
    int pid, rv;
    pid = wait(&rv);
    printf("%d child process end with exit code %d\n", pid, rv);
    childcount--;
}

int main(int argc, char *argv[])
{
    if (argc != 2)  {
        printf("Usage: ./calc_server [port no.]\n");
        exit(1);
    }

    if (atoi(argv[1]) < 45000 || atoi(argv[1]) > 65000)  {
        printf("port out of range\n");
        exit(0);
    }

    /*  After fork it will:
    *   *   Contains children process id in parent.
    *   *   Will be the last value of it before parent called fork in child .  */
    int pid = 0;
    int rv, op=0;

    /*  hints -> We will fill it with out server specifications
    *   (ip version, transport protocol, some flags). */
    struct addrinfo hints;

    /*  Firstly, we will clear (zeroing) the hints struct (for safety). Then,
    *   we will fill it with our configuration of the server.
    *   AF_INET -> IPv4     |     SOCK_STREAM -> TCP
    *   AI_PASSIVE -> We will use our this machine ip. */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    /* argv[1] is the port number you want the server to bind and listen to. */
    CreateSocket(NULL, argv[1], &hints, &listenfd);

    /*  Specify a signal handler for SIGCHLD signal */
    rv = signal(SIGCHLD, signal_handler);
    if (rv != 0)    {
        perror("server: signal");
        exit(1);
    }

    while(1)    {
        printf("\n\nwaiting for connections...\n\n");
        memset(&their_addr, 0, sizeof(their_addr));
        sin_size = sizeof their_addr;

        /*  accept() will return the client socket to clientfd.
        *   Also, it will fill their_addr with the IP address and port number of
        *   the peer of the connecion (client) in network byte order, and the
        *   length of it in sin_size. */
        clientfd = accept(listenfd, (struct sockaddr *)&their_addr, &sin_size);
        
        if (clientfd == -1)     {
            perror("accept");
            break;
        }

        pid = fork();
        if (pid == 0)   {
            close(listenfd);
            rv = client_handler();
            close(clientfd);
            exit(0);
        }

        childcount++;
        close(clientfd);  /* We don't need this, so close it.. */
    }
    for (int i = 0; i < childcount; i++)
        {pid = wait(&rv);}
    close(listenfd);

    printf("Parent Server listen socket closed\nFinished\n");
    exit(0);
}

/**
 * @brief 
 * 
 * @return int 
 */
int client_handler()
{
    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int rv, received = 0, result;
    enum operations choice = ERR;
    char *choices[] = {"ERROR", "ADD", "SUBSTRACT", "MULTIPLY", "DIVISION", "GPA", "EXIT"};

    /* Will contain the IP and Port in host byte order of client in their_addr */
    char peer_name[NI_MAXHOST], peer_port[NI_MAXSERV];

    /*  getnameinfo() will convert the IP:Port from network byte order to
    *   host byte order.
    *   The address will be saved as string in host byte order in peer_name
    *   and peer_port.*/
    getnameinfo((struct sockaddr *)&their_addr, sin_size, peer_name, sizeof(peer_name),
                peer_port, sizeof(peer_port), NI_NUMERICHOST | NI_NUMERICSERV);
    printf("server: got connection from %s:%s\n", peer_name, peer_port);

    while (1) {
        /* We zeroed (reseted) recv_buf, to clean it for the next data coming. */
        memset(recv_buf, 0, RECV_BUF_LEN);

        printf("waiting for client: %s:%s messages...\n", peer_name, peer_port);
        received = recv(clientfd, recv_buf, RECV_BUF_LEN, 0);
        if (received <= 0) {
            perror("server: recv");
            break;
        }

        if (recv_buf[0] >= '1' && recv_buf[0] <= '6')
            {choice = atoi(recv_buf[0]);}

        recv_buf[0] = ' ';
        printf("\nclient %s:%s:\tchoice: %s with message: \"%s\" from the\n",
                    peer_name, peer_port, choices[choice], recv_buf);

        if (choice == EXIT)   {
            printf("exiting\n");
            rv = 0;
            break;
        }
 
        else if (choice != ERR)  {
            /* Pass our handler the message. */
            if (choice == GPA)
                {rv = handle_gpa(recv_buf, &result, received);}
            else
                {rv = handle_msg(recv_buf, received, &result, choice);}
        }
        else    {rv = -4;}

        if (rv == -1)   {sprintf(send_buf, "Invalid values.");}
        else if (rv == -2)   {sprintf(send_buf, "Invalid opration.");}
        else if (rv == -3)   {sprintf(send_buf, "Division by zero.");}
        else if (rv == -4)   {sprintf(send_buf, "Invalid Choice.");}
        else if (rv == -5)   {sprintf(send_buf, "Invalid GPA List.");}
        else if (rv == 0)    {sprintf(send_buf, "%d", result);}

        rv = send(clientfd, send_buf, strlen(send_buf), 0);
        printf("Sending \"%s\" to the client\n\n", send_buf);
        if (rv == -1)   {
            perror("server: send");
            break;
        }
    }

    printf("Closing connection with %s:%s\n", peer_name, peer_port);
    return rv;
}
