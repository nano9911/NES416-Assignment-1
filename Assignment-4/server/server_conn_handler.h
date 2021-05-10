/**
 * @file server_conn_handler.h
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Connection Handler Header file for NES416/HW4
 * @date 2021-05-01
 * @version 0.1
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "server_socket_handler.h"
#include "server_message_handler.h"


int tcp_client_handler();

int handle_message( char *msg, int *len, char *output)    {

    int rv = decode_msg(msg, len, output);

    if (rv == ERR)   {sprintf(output, "Invalid choice.");}

    else if (rv == ENC || rv == DEC) {
        cipher(output, *len, rv);
    }

    else if (rv == LIST)    {
        list_directory(output);
    }

    else if (rv == EXIT)
        printf("Client choose Exit(3) and going to close\n");

    return 0;
}

/*****************************************************************/
/**
 * @brief handle udp messages came to main(), it will only be
 * called if the socket got a message and ready.
 * Will fork a child and prepare the appropriate variables for it.
 * 
 * @param sockfd UDP Listening socket which got a connection
 * @return int : status
 */
int udp_conn()    {
    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int received = 0, rv=0;

    /*  We zeroed (reseted) buffers, to clean it for the next
     *  data coming. */
    memset(recv_buf, 0, RECV_BUF_LEN);
    memset(send_buf, 0, SEND_BUF_LEN);
    memset(&their_addr, 0, sizeof(their_addr));
    sin_size = sizeof their_addr;

    received = Recvfrom(recv_buf, RECV_BUF_LEN, 0,
            &their_addr, &sin_size);

    /*  getnameinfo() will convert the IP:Port from network byte
     *   order to host byte order.
     *   The address will be saved as string in host byte order in
     *   peer_name and peer_port.
     */
    getnameinfo((struct sockaddr *)&their_addr, sin_size,
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);

    if (received == 0)  {
        printf("UDP Socket: Empty Message received from %s:%s and ignored\n\n",
            sender_addr, sender_svc);
    }

    printf("/*****************************************************************/\n");
    printf("UDP Socket: Message received from %s:%s: (length = %ld)\n%s\n",
            sender_addr, sender_svc, strlen(recv_buf), recv_buf);
    printf("/*****************************************************************/\n\n");

    rv = handle_message(recv_buf, &received, send_buf);
    if (rv == 1)    {
        return 0;
    }

    Sendto(send_buf, strlen(send_buf), 0,
        &their_addr, sin_size, sender_addr, sender_svc);

    return 0;
}

/*****************************************************************/
/**
 * @brief handle tcp connection came to main(), it will only be
 * called if the socket got connection and ready.
 * Will fork a child and prepare the appropriate variables for it.
 * 
 * @param sockfd TCP Listening socket which got a connection
 * @return int : status
 */
int tcp_conn()  {
    int rv=0, pid;
    memset(&their_addr, 0, sizeof(their_addr));
    sin_size = sizeof their_addr;

    tcpclientfd = Accept(&their_addr, &sin_size);

    /*  Creating a child process to handle the new accepted
    *   connection, and the parent will return to accept more
    *   new clients. */
    pid = fork();
    if (pid == 0)   {
        /*  child don't need that*/
        close(tcpsockfd);
        close(udpsockfd);
        rv = tcp_client_handler();
        close(tcpclientfd);
        exit(rv);
    }

    childcount++;
    /*  parent doesn't need that    */
    close(tcpclientfd);
    return 0;
}

/*****************************************************************/
/**
 * @brief child proccess function to handle tcp connection after
 * fork() in parent. From the start of this file we made the
 * required parameters global (out of main()), to get rid of any
 * parameters passing issues.
 * 
 * @return int, defines the state or the cause of the connection
 * termination process.
 */
int tcp_client_handler()
{
    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int received = 0, rv=EXIT_SUCCESS;

    /*  getnameinfo() will convert the IP:Port from network byte
    *   order to host byte order.
    *   The address will be saved as string in host byte order in
    *   peer_name and peer_port.*/
    getnameinfo((struct sockaddr *)&their_addr, sin_size,
                sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc),
                NI_NUMERICHOST | NI_NUMERICSERV);
    printf("TCP Server: Got connection from %s:%s\n", sender_addr, sender_svc);

    while (1) {
        /* We zeroed (reseted) recv_buf, to clean it for the next data coming. */
        memset(recv_buf, 0, RECV_BUF_LEN);
        memset(send_buf, 0, SEND_BUF_LEN);

        printf("TCP server: waiting for client: %s:%s messages...\n",
                                sender_addr, sender_svc);
        received = Recv(recv_buf, RECV_BUF_LEN, 0, sender_addr, sender_svc);

        if (received == 0)
            continue;

        rv = handle_message(recv_buf, &received, send_buf);
        if (rv != 0)
            break;

        Send(send_buf, strlen(send_buf), 0, sender_addr, sender_svc);
        rv = EXIT_SUCCESS;
    }

    printf("TCP server: Closing connection with %s:%s\n\n", sender_addr, sender_svc);
    return rv;
}
