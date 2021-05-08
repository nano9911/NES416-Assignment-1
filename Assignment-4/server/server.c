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

struct children {
    pid_t pid;
    struct children *next;
};

void signal_handler(int sig_no)   {
    if (sig_no == SIGCHLD)  {
        pid_t pid;
        int rv;
        pid = waitpid(-1, &rv, 0);
        printf("%d child process end with exit code %d\n", pid, rv);
        childcount--;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)  {
        printf("Usage: ./%s [port no.]\n", argv[0]);
        exit(0);
    }

    if (atoi(argv[1]) < 1024 || atoi(argv[1]) > 45000)  {
        printf("port out of range\n");
        exit(0);
    }

    sig_t sig;
    int rv=EXIT_SUCCESS, maxfd=0, chrv;

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

    /* Specify a signal handler for SIGCHLD signal */
    sig = signal(SIGCHLD, signal_handler);
    if (sig == SIG_ERR)    {
        perror("server: signal");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&rset);
    while(1)    {
        printf("\nwaiting for connections...\n\n");
        FD_SET(tcpsockfd, &rset);
        FD_SET(udpsockfd, &rset);
        maxfd = MAX(tcpsockfd, udpsockfd) + 1;

        if (select(maxfd, &rset, NULL, NULL, NULL) == -1)   {
            perror("main: select rset");
            continue;
        }

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

    /*  if parent ended unexpectedly, we should wait for live children to end    */
    for (int i = 0; i < childcount; i++)    {
        int pid = waitpid(-1, &chrv, 0);
        printf("%d child process end with exit code %d\n", pid, chrv);
        childcount--;
    }

    close(tcpsockfd);
    close(udpsockfd);
    printf("Parent Server listen socket closed\nFinished\n");
    exit(rv);
}
