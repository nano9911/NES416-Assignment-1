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

#include "client_wrappers.h"

/*
 *   CreateSocket() is our function to make it more readable and easier to handle,
 *   where we will pass to it the configuration parameters, and it will return
 *   the required socket which is based on our configuration, or it will end collect
 *   garbage, then exit() and end the program, so no need to check it,
 *   it won't return any value.
 */
int get_socket(char ip[], char port[], struct addrinfo *hints) {
    int sockfd;
    int rv;
    struct addrinfo *ptr, *res;

    /*  getadddrinfo() will take the configuration files specified previosly in hints
     *  and the IP and port number to get the possible parameters (addresses) to use.
     *  The returned addresses are in a linked list which the first pointer of it will be
     *  in the last parameter passed to getaddrinfo() (res).
     */
    rv = getaddrinfo(ip, port, hints, &res);
    if (rv != 0)    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    /*  We will loop through all the results and bind to the first we can create and
     *  connect to server with.
     *  socket() and connect() are not critical if failed, so we can just try again.
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

    /*  getnameinfo() will convert the IP:Port from network byte
     *   order to host byte order.
     *   The address will be saved as string in host byte order in
     *   peer_name (IP) and peer_port (Port #).
     */
    getnameinfo(ptr->ai_addr, ptr->ai_addrlen,
                peer_name, sizeof(peer_name),
                peer_port, sizeof(peer_port),
                NI_NUMERICHOST | NI_NUMERICSERV);

    freeaddrinfo(res); /* All done with this structure */

    if (ptr == NULL)    {
        fprintf(stderr, "get_socket: failed to connect\n");
        return -1;
    }

//    sprintf(peer_name, "%s", inet_ntoa((*peer).sin_addr));

    return sockfd;
}

/**
 * @brief Create a socket object
 * 
 * @param svc a string presents the service name, determines what port to use.
 * @param transport Transport Protocol (TCP=1, UDP=0).
 * @return int represents new socket descriptor.
 */
int create_socket(char addr[], char svc[], int transport) {
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
    hints.ai_socktype = (transport == 1 ? SOCK_STREAM : SOCK_DGRAM);

    return get_socket(addr, svc, &hints);
}
