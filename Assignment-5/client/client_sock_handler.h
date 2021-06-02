/**
 * @file client_socket_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Socket Handler Header file for NES416/HW4
 * @version 0.1
 * @date 2021-05-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef WRAPPER_INCLUDED
#include "client_wrappers.h"
#endif /*ifndef WRAPPER_INCLUDED*/

#define CLIENT_SOCK_HANDLER_INCLUDED

/**
 * @brief Get the socket object, is our function to make it more readable 
 * and easier to handle, where we will pass to it the configuration
 * parameters, and it will return the required socket which is based
 * on our configuration, or it will end collect garbage, then exit() and
 * end the program, so no need to check it, it won't return any value.
 * 
 * @param target 
 * @param svc 
 * @return int represents the new socket desriptor
 */
int get_socket(char target[], char svc[]) {
    int sockfd;
    int rv;
    /**
     * @brief hints -> We will fill it with out server specifications
     * (ip version, transport protocol, some flags). */
    struct addrinfo *ptr, *res, hints;

    /**
     * @brief Firstly, we will clear (zeroing) the hints struct (for safety). Then,
     * we will fill it with our configuration of the server.
     * AF_INET -> IPv4     |     SOCK_STREAM -> TCP
     * AI_PASSIVE -> We will use our this machine ip. */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    /**
     * @brief getadddrinfo() will take the configuration files specified previosly in hints
     * and the IP and port number to get the possible parameters (addresses) to use.
     * The returned addresses are in a linked list which the first pointer of it will be
     * in the last parameter passed to getaddrinfo() (res). */
    rv = getaddrinfo(target, svc, &hints, &res);
    if (rv != 0)    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    /**
     * @brief We will loop through all the results and bind to the first we can create and
     * connect to server with.
     * socket() and connect() are not critical if failed, so we can just try again.
     */
    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)     {
        sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sockfd == -1)     {
            perror("get_socket: socket");
            continue;
        }

        if (connect(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1)   {
            perror("get_socket: connect");
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(res); /* All done with this structure */

    if (ptr == NULL)    {
        fprintf(stderr, "get_socket: failed to connect\n");
        return -1;
    }

    return sockfd;
}
