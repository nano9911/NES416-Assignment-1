/**
 * @file HW1_server.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Main server file of NES416/HW4
 * @date 2021-05-01
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */


#include "conn_handler.h"
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <wait.h>

int sockfd;               /* TCP Server listening socket */

void signal_handler(int sig_no)   {
    pid_t pid, rv;
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

    if (atoi(argv[1]) < 1024 || atoi(argv[1]) > 45000)  {
        printf("port out of range\n");
        exit(0);
    }

    sig_t sig;
    int udpsockfd,          /* UDP Server listening socket  */
        maxfd=0, rv, op=0;
    pid_t pid = 0;          /* childs pid */

    fd_set  rset,           /* read set descriptors */
            wset;           /* write set descriptor */

    /*  argv[1] is the port number you want the server to bind and
    *   listen to. */
    /*  create_socket() determines if the socket type if it's
    *   TCP=1 or UDP=0 on the second argument. */

    /*TCP socket*/
    sockfd = create_socket(argv[1], 1);
    if (sockfd == -1)
        {perror("create_socket 1(tcp)"); exit(1);}
    /*UDP socket*/
    udpsockfd = create_socket(argv[1], 1);
    if (udpsockfd == -1)
        {perror("create_socket 0(udp)"); exit(1);}

    /* Specify a signal handler for SIGCHLD signal */
    sig = signal(SIGCHLD, signal_handler);
    if (sig == SIG_ERR)    {
        perror("server: signal");
        exit(1);
    }

    FD_ZERO(&rset);
    while(1)    {
        printf("\nwaiting for connections...\n\n");
        FD_SET(sockfd, &rset);
        FD_SET(udpsockfd, &rset);
        maxfd = max(sockfd, udpsockfd) + 1;

        rv = select(maxfd, &rset, NULL, NULL, NULL);
        if (rv == -1)   {
            perror("main: select rset");
            break;
        }

        if (FD_ISSET(sockfd, &rset))   {
            if (tcp_conn(sockfd) == -1)    {
                perror("main: tcp_conn");
                break;
            }
        }

        if (FD_ISSET(udpsockfd, &rset)) {
            if (udp_conn(udpsockfd) == -1)  {
                perror("main: udp_conn");
                break;
            }
        }
    }

    /*  if parent ended unexpectedly, we should wait for live children to end    */
    for (int i = 0; i < childcount; i++)    {
        pid = wait(&rv);
        printf("%d child process end with exit code %d\n", pid, rv);
    }

    close(sockfd);
    close(udpsockfd);
    printf("Parent Server listen socket closed\nFinished\n");
    exit(0);
}

