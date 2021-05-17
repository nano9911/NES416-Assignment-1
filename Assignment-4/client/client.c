/**
 * @file client.c
 * @author Adnan Hleihel (you@domain.com)
 * @brief Main client file of NES416/HW4
 * @version 0.1
 * @date 2021-05-07
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "client_socket_handler.h"
#include "client_message_handler.h"
#include <signal.h>

int create_message(char *output);

void signal_handler(int sig_no)   {
    if (sig_no == SIGINT)   {
        printf("Exiting: Interrupted signal handled: SIGINT(%d)\n", sig_no);
        close(sockfd);
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])    {
    int choice = check_input_and_return_choice(argc, argv);

    /* Specify a signal handler for SIGINT signal using
    sigaction struct and function */
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, NULL);

    int rv=0;
    /* message length should be 2 bytes than send buffer for the encoding */
    char  send_buf[SEND_BUF_LEN], recv_buf[RECV_BUF_LEN];

    /* create the socket based on user choice */
    sockfd = create_socket(argv[1], argv[2], choice);

    while (1)   {
        printf("%s\n%s\n%s\n%s\n", menu[0], menu[1], menu[2], menu[3]);
        /* Flushing buffers */
        memset(send_buf, 0, sizeof send_buf);
        memset(recv_buf, 0, sizeof recv_buf);

        rv = create_message(send_buf);

        Send(send_buf, strlen(send_buf)+1, 0);

        printf("/*****************************************************************/\n");
        printf("Sent message:(length = %ld)\n%s\n", strlen(send_buf), send_buf);
        printf("/*****************************************************************/\n\n");

        if (rv == EXIT) {
            printf("Closing Client program\n");
            sleep(0.5);
            break;
        }

        Recv(recv_buf, RECV_BUF_LEN, choice);

        printf("/*****************************************************************/\n");
        printf("Message received from %s:%s: (Length = %ld)\n%s\n",
                peer_name, peer_port, strlen(recv_buf), recv_buf);
        printf("/*****************************************************************/\n\n");
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Get the user choice object
 * 
 * @return int : user choice
 */
int get_user_choice()   {
    char user_choice;

    while(1)    {
        /* get user choice and check it */
        printf("Enter your choice number:\t");

        user_choice = getc(stdin);

        if (atoi(&user_choice) < 1 || atoi(&user_choice) > 4)  {
            fprintf(stderr, "Invalid choice\n");
            continue;
        }
        break;
    }

    return atoi(&user_choice);
}

/**
 * @brief Create a message object after taking user input.
 *
 * Note: The message will contain user choice in the first byte
 * then the actual message starts from the second byte.
 * 
 * @param msg string pointer to send_buf
 * @return int: status
 */
int create_message(char *msg)    {
    int rv = get_user_choice();

    char output[SEND_BUF_LEN-2];
    memset(output, 0, sizeof output);

    /* handle user choice */
    if (rv == ENC || rv == DEC) {
        printf("Enter %s:\t", type[rv]);

        scanf("%s", output);
        cipher(output, strlen(output), output, rv);
        sprintf(msg, "%d%s", rv, output);
    }

    else if (rv == EXIT)    {
        printf("Enter remination message to send to server:\n");
        printf("([--] to terminate the server also)\n");

        scanf("%s", output);
        sprintf(msg, "%d%s", rv, output);
    }

    else if (rv == LIST)    {
        sprintf(msg, "%d", rv);
    }

    return rv;
}
