/**
 * @file socket_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Header file of NES416/HW1
 * @date 2021-03-18
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
void CreateSocket(char ip[], char port[], struct addrinfo *hints, int *listenfd) {
    /* sockfd will be used in the function and will passes to the listenfd 
        if succeeded. */
    int sockfd;
    int rv;
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
        sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sockfd == -1)     {
            perror("server: socket");
            continue;
        }

        rv = bind(sockfd, ptr->ai_addr, ptr->ai_addrlen);
        if (rv == -1)   {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        rv = listen(sockfd, BACKLOG);
        if (rv == -1)     {
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
    *listenfd = sockfd;
}
