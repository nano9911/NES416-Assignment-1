/**
 * @file HW1_server.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief Main server file of NES416/HW2
 * @date 2021-03-17
 *
 */


#include "socket_handler.h"
#include "message_handler.h"
#include <signal.h>
#include <wait.h>

#define RECV_BUF_LEN 512        // Receive buffer size in bytes
#define SEND_BUF_LEN 512         // Send buffer size in bytes

int sockfd,               /* Server listening socket */
    tcpclientfd;               /* Client handler socket */

int childcount = 0;

/* Will be filled with the address of the peer connection (client ip and port). */
struct sockaddr_storage their_addr;

/* Will contain the length of the returned address from aacept() syetem call. */
socklen_t sin_size = sizeof(their_addr);

char menu[] = "1) Add.\n2) Substract.\n3) Multiply.\n4) Divide.\n5) Compute GPA.\n6) Exit.";

int client_handler();

void signal_handler(int sig_no)   {
    pid_t pid, rv;
    pid = wait(&rv);
    printf("%d child process end with exit code %d\n", pid, rv);
    childcount--;
}

int main(int argc, char *argv[])
{
    if (argc != 2)  {
        printf("Usage: ./calc_server [port no.]\n");
        exit(1);
    }

    if (atoi(argv[1]) < 45000 || atoi(argv[1]) > 65000)  {
        printf("port out of range\n");
        exit(0);
    }

    /*  After fork it will:
    *   *   Contains children process id in parent.
    *   *   Will be the last value of it before parent called fork in child .  */
    pid_t pid = 0;
    sig_t sig;
    int rv, op=0;

    /*  hints -> We will fill it with out server specifications
    *   (ip version, transport protocol, some flags). */
    struct addrinfo hints;

    /*  Firstly, we will clear (zeroing) the hints struct (for safety). Then,
    *   we will fill it with our configuration of the server.
    *   AF_INET -> IPv4     |     SOCK_STREAM -> TCP
    *   AI_PASSIVE -> We will use our this machine ip. */
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    /* argv[1] is the port number you want the server to bind and listen to. */
    CreateSocket(NULL, argv[1], &hints, &sockfd);

    /*  Specify a signal handler for SIGCHLD signal */
    sig = signal(SIGCHLD, signal_handler);
    if (sig == SIG_ERR)    {
        perror("server: signal");
        exit(1);
    }

    while(1)    {
        printf("\nwaiting for connections...\n\n");
        memset(&their_addr, 0, sizeof(their_addr));
        sin_size = sizeof their_addr;

        /*  accept() will return the client socket to clientfd.
        *   Also, it will fill their_addr with the IP address and port number of
        *   the peer of the connecion (client) in network byte order, and the
        *   length of it in sin_size. */
        tcpclientfd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        
        if (tcpclientfd == -1)     {
            perror("accept");
            break;
        }

        /*  Creating a child process to handle the new accepted connection, and the parent
        *   will return to accept more new clients. */
        pid = fork();
        if (pid == 0)   {
            /*  child don't need that*/
            close(sockfd);
            rv = client_handler();
            close(tcpclientfd);
            exit(rv);
        }

        childcount++;
        /*  parent doesn't need that    */
        close(tcpclientfd);
    }
    /*  if parent ended in an unpredictable way, we should wait for live children to end    */
    for (int i = 0; i < childcount; i++)    {
        pid = wait(&rv);
        printf("%d child process end with exit code %d\n", pid, rv);
    }

    close(sockfd);
    printf("Parent Server listen socket closed\nFinished\n");
    exit(0);
}

/**
 * @brief child proccess function to handle connection before fork() in parent.
 *  from the start of this file we made the required parameters global (out of main()),
 *  to get rid of any parameters passing issues.
 * 
 * @return int, defines the state or the cause of the connection termination process.
 */
int client_handler()
{
    char recv_buf[RECV_BUF_LEN], send_buf[SEND_BUF_LEN];
    int rv, received = 0;
    float result;

    /*  choice is the first character from the client message   */
    int choice = ERR;
    char *choices[] = {"ERROR", "ADD", "SUBSTRACT", "MULTIPLY", "DIVISION", "GPA", "EXIT"};

    /* Will contain the IP and Port in host byte order of client in their_addr */
    char sender_addr[NI_MAXHOST], sender_svc[NI_MAXSERV];

    /*  getnameinfo() will convert the IP:Port from network byte order to
    *   host byte order.
    *   The address will be saved as string in host byte order in peer_name
    *   and peer_port.*/
    getnameinfo((struct sockaddr *)&their_addr, sin_size, sender_addr, sizeof(sender_addr),
                sender_svc, sizeof(sender_svc), NI_NUMERICHOST | NI_NUMERICSERV);
    printf("server: got connection from %s:%s\n", sender_addr, sender_svc);

    /*  send the menu of options the server can offer to client, so the user can
    *   choose from the client side */
    rv = send(tcpclientfd, menu, sizeof(menu), 0);
    if (rv == -1)   {
        perror("server: send menu");
        return -1;
    }

    while (1) {
        /* We zeroed (reseted) recv_buf, to clean it for the next data coming. */
        memset(recv_buf, 0, RECV_BUF_LEN);

        printf("waiting for client: %s:%s messages...\n", sender_addr, sender_svc);
        received = recv(tcpclientfd, recv_buf, RECV_BUF_LEN, 0);
        if (received <= 0) {
            perror("server: recv");
            break;
        }

        /*  check user choice field, if it's valid we will assign it to "choice" variable,
        *   and replace it with space to make the extarction process easier.    */
        if (recv_buf[0] >= '1' && recv_buf[0] <= '6')
            {choice = (int)recv_buf[0] - 48;}

        recv_buf[0] = ' ';
        printf("\nclient %s:%s:\tchoice: %s with message: \"%s\"\n",
                    sender_addr, sender_svc, choices[choice], recv_buf);

        if (choice == EXIT)   {
            printf("exiting\n");
            rv = 0;
            break;
        }
 
        else if (choice == ERR) {rv = -4;}
        /* Pass the appropriate handler the message. */
        else if (choice == GPA)
            {rv = handle_gpa(recv_buf, &result, received);}
        else
            {rv = handle_msg(recv_buf, received, &result, choice);}

        if (rv == -1)   {sprintf(send_buf, "Invalid values.");}
        else if (rv == -2)   {sprintf(send_buf, "Invalid opration.");}
        else if (rv == -3)   {sprintf(send_buf, "Division by zero.");}
        else if (rv == -4)   {sprintf(send_buf, "Invalid Choice.");}
        else if (rv == -5)   {sprintf(send_buf, "Invalid GPA List.");}
        else if (rv == 0)    {sprintf(send_buf, "%f", result);}

        rv = send(tcpclientfd, send_buf, strlen(send_buf), 0);
        printf("Sending \"%s\" to the client\n\n", send_buf);
        if (rv == -1)   {
            perror("server: send result");
            break;
        }
    }

    printf("Closing connection with %s:%s\n", sender_addr, sender_svc);
    return rv;
}
