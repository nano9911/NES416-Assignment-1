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


#include "server_conn_handler.h"

void signal_handler(int sig_no);

int main(int argc, char *argv[])
{
    if (argc != 2)  {
        printf("Usage: ./%s [service name]\n", argv[0]);
        exit(0);
    }

    /* Specify a signal handler for SIGCHLD and SIGINT signal using sigaction struct and function */
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGCHLD, &sigIntHandler, NULL);
    sigaction(SIGINT, &sigIntHandler, NULL);

    int rv=EXIT_SUCCESS, maxfd=0;

    fd_set  rset;           /* read set descriptors */

    /*  argv[1] is the port number you want the server to bind and
    *   listen to. */
    /*  create_socket() determines if the socket type if it's
    *   TCP=1 or UDP=0 on the sebest of cond argument. */

    /*TCP socket*/
    tcpsockfd = create_socket(argv[1], 1);
    if (tcpsockfd == -1)
        {perror("create_socket 1(tcp)"); exit(EXIT_FAILURE);}
    /*UDP socket*/
    udpsockfd = create_socket(argv[1], 0);
    if (udpsockfd == -1)
        {perror("create_socket 0(udp)"); exit(EXIT_FAILURE);}

    printf("\nwaiting for connections...\n\n");

    FD_ZERO(&rset);
    while(1)    {
        FD_SET(tcpsockfd, &rset);
        FD_SET(udpsockfd, &rset);
        maxfd = MAX(tcpsockfd, udpsockfd) + 1;

//        select(maxfd, &rset, NULL, NULL, NULL);
        if (select(maxfd, &rset, NULL, NULL, NULL) == -1)
            continue;

        if (FD_ISSET(tcpsockfd, &rset) > 0)   {
            if (tcp_conn() == -1)    {
                perror("main: tcp_conn");
                rv = EXIT_FAILURE;
                break;
            }
        }

        if (FD_ISSET(udpsockfd, &rset) > 0) {
            if (udp_conn() == -1)  {
                perror("main: udp_conn");
                rv = EXIT_FAILURE;
                break;
            }
        }
    }

    wait_children_and_exit(rv);
}

void signal_handler(int sig_no)   {
    if (sig_no == SIGCHLD)  {
        pid_t pid;
        int rv=0;

        pid = waitpid(-1, &rv, 0);
        printf("%d child process ended with exit code %d\n", pid, rv);
        childcount--;

        if (rv == QUIT || rv == 25344) {
            printf("TCP Client sent termination signal \"3--\"to child %d so \
parent is going down too now.\n", pid);

        wait_children_and_exit(EXIT_SUCCESS);
        }
        return;
    }

    if (sig_no == SIGINT)   {
        printf("Exiting with out waiting childs: Interrupte signal: SIGINT(%d)\n", sig_no);
        close(tcpclientfd);
        close(tcpsockfd);
        close(udpsockfd);
        exit(EXIT_SUCCESS);
    }
}
