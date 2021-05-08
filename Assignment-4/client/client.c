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

static int create_message(char *output);

int main(int argc, char *argv[])    {
    int choice = check_input_and_return_choice(argc, argv);

    int rv=EXIT_SUCCESS;
    /* message length should be 2 bytes than send buffer for the encoding*/
    char send_buf[SEND_BUF_LEN], recv_buf[RECV_BUF_LEN];

    /* create the socket based on user choice */
    sockfd = create_socket(argv[1], argv[2], choice);

    while (1)   {
        printf("%s\n%s\n%s\n%s\n", menu[0], menu[1], menu[2], menu[3]);
        /* Flushing buffers */
        memset(send_buf, 0, sizeof send_buf);
        memset(recv_buf, 0, sizeof recv_buf);

        rv = create_message(send_buf);
        if (rv == EXIT || rv == ERR)
            break;
        /* Encoding the message:
         * First byte is the user choice, then the second is the text len */

        Send(send_buf, strlen(send_buf)+1, 0);

        printf("Sent message: %s\n", send_buf);

        Recv(recv_buf, RECV_BUF_LEN, choice);

        printf("From %s:%s Message received: %s\n\n\n", argv[1], argv[2], recv_buf);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Get the user choice object
 * 
 * @return int 
 */
int get_user_choice()   {
    char user_choice;

    while(1)    {
        /* get user choice and check it */
        printf("Enter your choice number:\t");
        user_choice = '\0';

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
 * @brief Create a message object after taking user input
 * 
 * @param output string pointer
 * @return int: status
 */
int create_message(char *msg)    {
    int rv = get_user_choice();
    char output[SEND_BUF_LEN-2];
    memset(output, 0, sizeof output);

    /* handle user choice */
    if (rv == ENC || rv == DEC) {
        printf("Enter message in plain text: ");

        scanf("%s", output);
        cipher(output, strlen(output), output, rv);
        sprintf(msg, "%d%s", rv, output);
    }

    else if (rv == EXIT)
        sprintf(output, "%d", EXIT);

    printf("message: %s\tchoice: %d\tlength: %lx\n", output, rv, strlen(output));
    return rv;
}
