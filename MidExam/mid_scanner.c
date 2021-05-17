/**
 * @file mid_scanner.c
 * @author Adnan Hleihel (JUST ID: 123423)
 * @brief NES416 MidTerm Exam Solution
 * @date 2021-05-04
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

void test_port(char ip[], char port[], struct addrinfo *hints);

int unreachable=1;

int main(int argc,char ** argv)
{
    if(argc != 4)   {
        printf("\nUsage: ./%s [target ip] [port range start] [port range end]\n", argv[0]);
        exit(0);
    }

    if (atoi(argv[2]) < 1 || atoi(argv[2]) > 65000 ||
        atoi(argv[3]) < 1 || atoi(argv[3]) > 65000 ||
        atoi(argv[2]) > atoi(argv[3]))   {

        printf("Invalid port range\n");
        exit(0);
    }
 
    int rv;
    char service[10];
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    printf("Open TCP port on %s are:", argv[1]);
    for (int i = atoi(argv[2]); i <= atoi(argv[3]); i++) {
        memset(service, 0, sizeof service);
        sprintf(service, "%d", i);
        test_port(argv[1], service, &hints);
    }

    if (unreachable == 1)   {
        printf("\tnone\nDestination down or unreachable.");
    }

    printf("\n\n");

    exit(0);
}


void test_port(char ip[], char port[], struct addrinfo *hints) {
    int sockfd;
    int rv;
    struct addrinfo *ptr, *res;

    rv = getaddrinfo(ip, port, hints, &res);
    if (rv != 0)    {
        printf("Couldn't resolve host ip\n");
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(ptr = res; ptr != NULL; ptr = ptr->ai_next)     {
        sockfd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (sockfd == -1)     {
            continue;
        }

        rv = connect(sockfd, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (rv == -1) {
            close(sockfd);
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    if (ptr != NULL)    {
        printf("\t%s", port);
        unreachable=0;
        close(sockfd);
    }
}
