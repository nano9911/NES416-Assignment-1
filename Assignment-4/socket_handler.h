/**
 * @file socket_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Header file of NES416/HW4
 * @date 2021-05-01
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

#define BACKLOG 10              // Pending connections queue capacity

/*
*   CreateSocket() is our function to make it more readable and easier to handle,
*   where we will pass to it the configuration parameters, and it will return
*   the listen socket which is based on our configuration, or it will end collect
*   garbage, then exit() and end the program, so no need to check it,
*   it won't return any value.
*
*   Finally the integer listenfd which will be our listening socket descriptor
*   when the function finishes, it will pass the created socket to it.
*/
int get_socket(char ip[], char port[], struct addrinfo *hints) {
    /* sockfd will be used in the function and will passes to the listenfd 
        if succeeded. */
    int sockfd;
    int rv,optval = 1;
    struct addrinfo *ptr, *res;

    /* getadddrinfo() will take the configuration files specified previosly in hints
        and the IP and port number to get the possible parameters (addresses) to use.
        The returned addresses are in a linked list which the first pointer of it will be
        in the last parameter passed to getaddrinfo() (res). */
    rv = getaddrinfo(ip, port, hints, &res);
    if (rv != 0)    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    /*
    **  We will loop through all the results and bind to the first we can create and
    **  bind to.
    **  socket() and bind() are not critical if failed, so we can just try again.
    **  But listen() is critical and it's failure means we should close the socket and exit.
    */
    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)     {
        if (socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol) == -1)     {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)   {
            perror("setsockopt SOL_SOCKET, SO_REUSEPOR");
            close(sockfd);
            exit(1);
        }

        if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1)   {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        if (listen(sockfd, BACKLOG) == -1)     {
            perror("server: listen");
            close(sockfd);
            exit(1);
        }

        break;
    }

    freeaddrinfo(res); /* All done with this structure */

    if (ptr == NULL)    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }
    /* No error checking here, because it won't reach that
        level unless it's created succesfully. */
    return sockfd;
}


int create_socket(char svc[], int transport) {
    if (transport != 0 && transport != 1)   {
        fprintf(stderr, "create_socket: invalid choice. (tcp = 1, udp = 0)\n");
        return -1;
    }
    /*  hints -> We will fill it with out server specifications
    *   (ip version, transport protocol, some flags). */
    struct addrinfo hints;

    /*  Firstly, we will clear (zeroing) the hints struct (for safety). Then,
    *   we will fill it with our configuration of the server.
    *   AF_INET -> IPv4     |     SOCK_STREAM -> TCP
    *   AI_PASSIVE -> We will use our this machine ip. */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = transport == 1 ? SOCK_STREAM : SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    return get_socket(NULL, svc, &hints);
}