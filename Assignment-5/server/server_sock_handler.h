/**
 * @file server_conn_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief server tcp connections handler header
 * @version 0.1
 * @date 2021-05-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once

#include "server_wrappers.h"
#define BACKLOG 10              // Pending connections queue capacity

/**
 * @brief Get the tcp socket discreptor, get_tcp_socket() is our function to make it
 * more readable and easier to handle, and it will return the listen socket which
 * is based on our configuration, or it will end then collect garbage, and end
 * the program, so no need to check it, it won't return any value.
 * 
 * @param ip : char[]
 * @param port : char[]
 * @return int : tcp socket discreptor
 */
int get_tcp_socket(char *ip, char *port) {
    int sockfd;
    int rv, optval=1;

    /*  hints -> We will fill it with out server specifications
    *   (ip version, transport protocol, some flags). */
    struct addrinfo hints, *ptr, *res;

    /*  Firstly, we will clear (zeroing) the hints struct (for safety). Then,
    *   we will fill it with our configuration of the server.
    *   AF_INET -> IPv4     |     SOCK_STREAM -> TCP
    *   AI_PASSIVE -> We will use this machine's ip. */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    /*  getadddrinfo() will take the configuration files specified previosly in hints
     *  and the IP and port number to get the possible parameters (addresses) to use.
     *  The returned addresses are in a linked list which the first pointer of it will be
     *  in the last parameter passed to getaddrinfo() (res).
     */
    rv = getaddrinfo(ip, port, &hints, &res);
    if (rv != 0)    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    /*  We will loop through all the results and bind to the first we can create and
     *  bind to.
     *  socket() and bind() are not critical if failed, so we can just try again.
     *  But listen() is critical and it's failure means we should close the socket and exit.
     */
    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)     {
        sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sockfd == -1)     {
            perror("get_socket: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)   {
            perror("get_socket: setsockopt SOL_SOCKET, SO_REUSEPOR");
            close(sockfd);
            return -1;
        }

        if (bind(sockfd, ptr->ai_addr, ptr->ai_addrlen) == -1)   {
            close(sockfd);
            perror("get_socket: bind");
            continue;
        }

        if (listen(sockfd, BACKLOG) == -1)     {
            perror("get_socket: listen");
            close(sockfd);
            return -1;
        }

        break;
    }

    freeaddrinfo(res); /* All done with this structure */

    if (ptr == NULL)    {
        fprintf(stderr, "get_socket: failed to bind\n");
        return -1;
    }

    /* No error checking here, because it won't reach that
        level unless it's created succesfully. */
    return sockfd;
}
