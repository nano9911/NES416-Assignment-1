/**
 * @file sockopt.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief NES416-Assignment#3, get all sockets options and
 * 			change SO_SNDLOWAT and SO_RCVLOWAT values.
 * 		This is the main file.
 * @date 2021-04-17
 * 
 */

#include "options.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>

// in case of solaris, and you used fcntl(), uncomment this line
//
#ifdef SUN
#include <unistd.h>
#include <stropts.h>
#include <sys/file.h>
#endif

int socket_nonblock_option();
int socket_nonblock_ioctl();
int socket_nonblock_fcntl();
void get_option(struct sock_opts *ptr);

int main(int argc, char **argv)
{
	int	sockfd, rv=0, level=-1,setsnd=32, setrcv=128;
	struct sock_opts *ptr;
	struct utsname buf;
	
	if (uname(&buf) == -1)	{
		perror("uname");
		exit(1);
	}

	printf("System info:\n%s %s %s %s %s\n\n", buf.sysname, buf.nodename, buf.release, buf.version, buf.machine);

	/* loop on all options and test if they're available */
	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
		if (ptr->opt_level != level)	{
			level = ptr->opt_level;
			switch (level)	{
				case SOL_SOCKET:	printf("SOL_SOCKET:\n"); break;
				case IPPROTO_IP:	printf("IPPROTO_IP:\n"); break;
				case IPPROTO_IPV6:	printf("IPPROTO_IPV6:\n"); break;
				case IPPROTO_TCP:	printf("IPPROTO_TCP:\n"); break;
				case IPPROTO_SCTP:	printf("IPPROTO_SCTP:\n"); break;
			}
		}
		get_option(ptr);
	}

	/*************************************************************/
   	/* To create a non blocking AF_INET stream socket, 			 */
	/* we have two options:	     								 */
	/*							     							 */
	/* - with SOCK_STREAM | SOCK_NONBLOCK, in this case we won't */
	/* use ioctl() or fcntl().				     				 */
//	sockfd = socket_nonblock_option();
	/*															 */
	/*															 */
	/* - with SOCK_STREAM, in this case we will use ioctl()      */
	sockfd = socket_nonblock_ioctl();
	/*															 */
	/* or fcntl().						     					 */
//	sockfd = socket_nonblock_fcntl();
	/*															 */
	/*															 */
	/* Important:												 */
	/* Please don't uncomment ioctl() and fctl() in the same 	 */
	/* code, I didn't really faced an issue while testing it,	 */
	/* but also I don't know what will go wrong.  			  	 */
	/*															 */
	/* - uncomment one to use the other one			     		 */
   	/*************************************************************/

	printf("\nTrying setsockopt(SO_SNDLOWAT): ");
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &setsnd, sizeof(int)) == -1)	{
		perror("\nsetsockopt SOL_SOCKET, SO_SNDLOWAT");
		printf("errno = %d\n", errno);
	}
	else	{printf("Succeeded\n");}

	/* get SO_SNDLOWAT 											 */
	printf("Current SO_SNDLOWAT value: ");
	*ptr = sock_opts[10];
	get_option(ptr);

	printf("\nTrying setsockopt(SO_RCVLOWAT): ");
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &setrcv, sizeof(int)) == -1)	{
		perror("\nsetsockopt SOL_SOCKET, SO_RCVLOWAT");
		printf("errno = %d\n", errno);
	}
	else	{printf("Succeeded\n");}

	/*															 */
	/* get SO_RCVLOWAT											 */
	printf("Current SO_RCVLOWAT value: ");
	*ptr = sock_opts[9];
	get_option(ptr);

	close(sockfd);
	exit(0);
}


int socket_nonblock_option()	{
	int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (sockfd == -1)	{
		perror("socket AF_INET, SOCK_STREAM | SOCK_NONBLOCK");
		exit(0);
	}

	return sockfd;
}

int socket_nonblock_ioctl()	{
	int sockfd, on=1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
		perror("socket AF_INET, SOCK_STREAM");
		exit(0);
	}
#if defined linux || defined freebsd
	if (ioctl(sockfd, FIONBIO, (char *)&on) == -1)	{
		perror("ioctl FIONBIO, 1");
		printf("errno = %d\n", errno);
	}
	else	{printf("\n\nioctl FIONBIO, 1: succeeded\n");}
#elif defined SUN
	if (ioctl(sockfd, _IONBF, (char *)&on) == -1)	{
		perror("ioctl _IONBF, 1");
		printf("errno = %d\n", errno);
	}
	else	{printf("\n\nioctl _IONBF, 1: succeeded\n");}
#endif
	return sockfd;
}

int socket_nonblock_fcntl()	{
	int sockfd, flags;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
		perror("socket AF_INET, SOCK_STREAM");
		exit(0);
	}

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)	{
		perror("fcntl F_GETFL");
		printf("errno = %d\n", errno);		
	}

#if defined linux || defined freebsd
	/* In case of Linux or FreeBSD use that if statement, and	 */
	/* uncomment the next one.									 */
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)	{
		perror("fcntl F_SETFL, O_NONBLOCK");
		printf("errno = %d\n", errno);
	}
	else	{printf("\n\nfcntl F_SETFL, O_NONBLOCK: succeeded\n");}

#elif defined solaris
	/* In case of Solaris uncomment this if statement, and		 */
	/* comment out the past if statement						 */
	if (fcntl(sockfd, F_SETFL, flags | FNDELAY) == -1)	{
		perror("fcntl F_SETFL, FNDELAY");
		printf("errno = %d\n", errno);
	}
	else 	{printf("\n\nfcntl F_SETFL, FNDELAY: succeeded\n");}
#endif

	return sockfd;
}


void get_option(struct sock_opts *ptr)	{
	int sockfd;
	socklen_t sin_size;
	char err[50];

	printf("\t%s: ", ptr->opt_str);

	if (ptr->opt_val_str == NULL)
		printf("(undefined)\n");

	else {
		switch(ptr->opt_level) {
			case SOL_SOCKET:
			case IPPROTO_IP:
			case IPPROTO_TCP:
				sockfd = socket(AF_INET, SOCK_STREAM, 0);
				break;
		#ifdef	IPPROTO_IPV6
			case IPPROTO_IPV6:
				sockfd = socket(AF_INET6, SOCK_STREAM, 0);
				break;
		#endif
		#ifdef	IPPROTO_SCTP
			case IPPROTO_SCTP:
				sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
				break;
		#endif
			default:
				sprintf(err, "unknown %d level, from opt_name: %s", ptr->opt_level, ptr->opt_str);
				perror(err);
					return;
			}

			sin_size = sizeof(val);
			if (getsockopt(sockfd, ptr->opt_level, ptr->opt_name, &val, &sin_size) == -1) {
				perror("getsockopt error");
			} else {
				printf("default = %s\n", (*ptr->opt_val_str)(&val, sin_size));
			}
		close(sockfd);
	}
	return;
}
