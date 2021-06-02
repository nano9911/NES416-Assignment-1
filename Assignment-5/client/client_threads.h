/**
 * @file client_threads.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief client threads implementation in this file
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef WRAPPER_INCLUDED
#include "client_wrappers.h"
#endif /*ifndef WRAPPER_INCLUDED*/

#ifndef CLIENT_SOCK_HANDLER_INCLUDED
#include "client_sock_handler.h"
#endif

void thread_cleanup(void *argv)   {
    struct thread_cleanup_arguments *args = 
        (struct thread_cleanup_arguments *)argv;
    
    close(args->sockfd);
    printf("Thread %d (ID: %ld) terminated and \
cleanedup correctly\n", args->id, args->tid);
}

/**
 * @brief thread routine to send the number user entered in the
 * main thread to the server, then receive the response from
 * server and print it
 * 
 * @param argv 
 */
void *client_thread(void *argv)  {
    struct thread_arguments *args =
        (struct thread_arguments *) argv;

    char    server_addr[NI_MAXHOST],
            svc_number[NI_MAXSERV],
            send_buf[SEND_BUF_LEN],
            recv_buf[RECV_BUF_LEN];

    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof(their_addr);

    struct thread_cleanup_arguments cleanup_args;
    pthread_cleanup_push(thread_cleanup, (void *)&cleanup_args);

    int sockfd = get_socket(args->target_addr,
                            args->target_svc);

    /*prepare cleanup routine arguments*/
    cleanup_args.tid = pthread_self();
    cleanup_args.id = args->id;
    cleanup_args.sockfd = sockfd;

    if (sockfd == -1)
        pthread_exit(NULL);

    getpeername(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    /**
     * @brief getnameinfo() will convert the IP:Port from network byte
     * order to host byte order.
     * The address will be saved as string in host byte order in
     * peer_name (IP) and peer_port (Port #).   */
    getnameinfo((struct sockaddr*)&their_addr, sin_size,
                server_addr, sizeof(server_addr),
                svc_number, sizeof(svc_number),
                NI_NUMERICHOST | NI_NUMERICSERV);

    printf("(Thread %d) Connected to server %s:%s\n\n",
                    args->id, server_addr, svc_number);

    sprintf(send_buf, "%ld", args->num);
    if (Send(sockfd, send_buf, strlen(send_buf), 0) == -1)
        pthread_exit(NULL);

    if (Recv(sockfd, recv_buf, sizeof recv_buf, 0) == -1)
        pthread_exit(NULL);

    printf("(Thread %d) Received response from server %s:%s:\n%s\n\n",
                    args->id, server_addr, svc_number, recv_buf);

    pthread_cleanup_pop(1);
    return 0;
}
