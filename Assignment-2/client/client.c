/**
 * @file client.c
 * @author Rou'a Kofahi (JUST ID: 126474)
 * @brief Main client file of NES416/HW1
 * @date 2021-03-21
 * 
 */

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SEND_BUF_LEN 100
#define RECV_BUF_LEN 100
#define MENU_BUF_LEN 100

int main(int argc,char ** argv)
{
    struct sockaddr_in sa, lo;
    int received, sent;         /*counters for bytes sent per each send() or recv()*/
    int conntfd,sockfd;                  /*discriptors to reach the sockets*/
    char send_buf[SEND_BUF_LEN];                      /*buffer to write received data in*/
    char recv_buf[RECV_BUF_LEN];                      /*buffer to write sent data in*/
    char menu_buf[MENU_BUF_LEN];                      /*buffer to save menu from server in*/
    char choice[2], expression[SEND_BUF_LEN-1];
    char *rv = 0;

    if(argc !=3)                         /*checking the number of arguments entered by the user*/
    {
        /*  argv[0]: ./client |   argv[1]: server ip |  argv[2]: server_port number  */
        printf("\nUsage: ./client [server_port] [server_port_no]\n");
        exit(0);
    }

    int port=atoi(argv[2]);

    sockfd=socket(AF_INET,SOCK_STREAM,0); /*socket creation and varification*/
    if(sockfd < 0)
    {
        printf("\nERROR IN SOCKET CREATION\n");
        exit(1);
    }

    bzero(&sa,sizeof(sa));               /*making sure that sa's data is erased */

    // assigning PORT and IP address 
    sa.sin_family=AF_INET;
    sa.sin_port=htons(port);
    if(inet_pton(AF_INET,argv[1],&sa.sin_addr) <= 0) /* converting Ipv4/Ipv6 string to binary */
    {
        printf("\nERROR IN IP ADDRESS\n");
        exit(1);
    }

    if(connect(sockfd,(struct sockaddr*)&sa,sizeof(sa)) < 0) /*connecting the server socket with the client socket*/
    {
        printf("\nERROR IN CONNECTION \n");
        exit(1);
    }

    received = recv(sockfd, menu_buf, MENU_BUF_LEN, 0);
    if (received <= 0)
    {
        printf("\nERROR IN RECV\n");
    }

    while (1)  {
        memset(send_buf, 0, SEND_BUF_LEN);
        memset(recv_buf, 0, RECV_BUF_LEN);
        memset(expression, 0, sizeof expression);
        memset(choice, 0, 2);

        printf("Menu from server:\n%s\n", menu_buf);

        printf("\nplease enter your choice first then your expression:\n");
        rv = fgets(expression, sizeof expression, stdin);
        if (rv == NULL)  /*getting user input*/
        {
            printf("\nERROR IN FGETS\n");
            break;
        }

        expression[strlen(expression)-1] = '\0';

        sprintf(send_buf, "%s", expression);

        sent = send(sockfd, send_buf, strlen(send_buf), 0);     /*sending user message to server*/
        if (sent == -1)
        {
            printf("\nERROR IN SEND\n");
            break;
        }

        if (send_buf[0] == '6')   /*checking if user want to end connection (exit)*/
        {
            printf("exiting\n");
            break;
        }

        received = recv(sockfd, recv_buf, RECV_BUF_LEN, 0);
        if (received <= 0)
        {
            printf("\nERROR IN RECV\n");
            break;
        }

        printf("%s = %s (%s:%s)\n", send_buf, recv_buf, argv[1], argv[2]);
    }

    close(sockfd);
    exit(0);
}
