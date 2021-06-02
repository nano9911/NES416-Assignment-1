/**
 * @file client.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief NES416 Assignment#5 client main file
 * @version 0.1
 * @date 2021-06-01
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "client_wrappers.h"
#include "client_sock_handler.h"
#include "client_threads.h"

int main(int argc, char *argv[]) {
    if (argc != 3)  {
        printf("Usage: %s [server address] [service name or number]", argv[0]);
    }

    if (atoi(argv[2]) < 0 || atoi(argv[2]) > 65535) {
        fprintf(stderr, "Invalid port [service] number\n\n");
        exit(0);
    }

    struct thread_arguments args1 = {argv[1], argv[2], 0, 1},
                            args2 = {argv[1], argv[2], 0, 2};

    printf("Enter the number you want the server to test if it's prime or not:\n");
    scanf("%ld", &(args1.num));
    args2.num = args1.num + 1;

    if (pthread_create(&t1, NULL, client_thread, (void *)&args1) != 0)    {
        perror("main: pthread_create");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&t2, NULL, client_thread, (void *)&args2) != 0)    {
        perror("main: pthread_create");
        printf("Cancelling thread 1, and going down\n");
        pthread_cancel(t1);
        exit(EXIT_FAILURE);
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    exit(0);
}
