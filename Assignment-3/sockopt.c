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

#ifdef __sun__
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
	int	sockfd, rv=0;
	socklen_t size=sizeof(int);
	/* used to set SO_SNDLOWAT and SO_RCVLOWAT values */
	int setsnd=32, setrcv=128;
	/* used to compare SO_SNDLOWAT and SO_RCVLOWAT values before setsokopt() and after*/
	int past_snd_val, past_rcv_val, cur_snd_val, cur_rcv_val;
	struct sock_opts *ptr;
	struct utsname buf;
	
	if (uname(&buf) == -1)	{
		perror("uname");
		exit(1);
	}

	printf("System info:\n%s %s %s %s %s\n\n", buf.sysname, buf.nodename,
	                                buf.release, buf.version, buf.machine);

	/* loop on all options and test if they're available */
	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++)
		{get_option(ptr);}

	/*************************************************************/
   	/* To create a non blocking AF_INET stream socket,           */
	/* we have two options:                                      */
	/*                                                           */
	/* - with SOCK_STREAM | SOCK_NONBLOCK, in this case we won't */
	/* use ioctl() or fcntl().                                   */
//	sockfd = socket_nonblock_option();
	/*                                                           */
	/*                                                           */
	/* - with SOCK_STREAM, with ioctl()                          */
//	sockfd = socket_nonblock_ioctl();
	/*                                                           */
	/* - with SOCK_STREAM, with fcntl()                          */
	sockfd = socket_nonblock_fcntl();
	/*                                                           */
	/*                                                           */
	/* Important:                                                */
	/* Please don't uncomment ioctl() and fctl() in the same     */
	/* code, I didn't really faced an issue while testing it,    */
	/* but also I don't know what will go wrong.                 */
	/*                                                           */
	/* - uncomment one to use the other one                      */
   	/*************************************************************/

   	/*************************************************************/
	/* getting current SO_SNDLOWAT and SO_RCVLOWAT values to     */
	/* compare after setsockopt() */
	if (getsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &past_snd_val, &size) == -1)
		{perror("\ngetsockopt SOL_SOCKET, SO_SNDLOWAT");}

	if (getsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &past_rcv_val, &size) == -1)
		{perror("\ngetsockopt SOL_SOCKET, SO_RCVLOWAT");}

	printf("\nPast values: SO_SNDLOWAT=%d, SO_RCVLOWAT=%d\n",past_snd_val ,past_rcv_val);
   	/*************************************************************/

	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &setsnd, sizeof(int)) == -1)
		{perror("\nsetsockopt SOL_SOCKET, SO_SNDLOWAT");}
	else	{
		printf("\nsetsockopt SOL_SOCKET, SO_SNDLOWAT: Succeeded.");
		if (getsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &cur_snd_val, &size) == -1)
			{perror("\ngetsockopt SOL_SOCKET, SO_SNDLOWAT");}

		if (past_snd_val == cur_snd_val)	{printf("But, value didn't change.\n");}
		printf("\nCurrent SO_SNDLOWAT value: %d\n", cur_snd_val);
//		*ptr = sock_opts[10];
//		get_option(ptr);
	}

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &setrcv, sizeof(int)) == -1)
		{perror("\nsetsockopt SOL_SOCKET, SO_RCVLOWAT");}
	else	{
		printf("\nsetsockopt SOL_SOCKET, SO_RCVLOWAT: Succeeded.");
		if (getsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &cur_rcv_val, &size) == -1)
			{perror("\ngetsockopt SOL_SOCKET, SO_RCVLOWAT");}

		if (past_rcv_val == cur_rcv_val)	{printf(" But, value didn't change.\n");}
		printf("\nCurrent SO_RCVLOWAT value: %d\n", cur_rcv_val);
//		*ptr = sock_opts[9];
//		get_option(ptr);
	}


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
#if (defined __linux__ || defined __FreeBSD__)
	if (ioctl(sockfd, FIONBIO, (char *)&on) == -1)	{
		perror("ioctl FIONBIO, 1");
		printf("errno = %d\n", errno);
	}
	else	{printf("\n\nioctl FIONBIO, 1: succeeded\n");}
#elif defined __sun__
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

#if (defined __linux__ || defined __FreeBSD__)
	/* In case of Linux or FreeBSD use that if statement, and	 */
	/* uncomment the next one.									 */
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)	{
		perror("fcntl F_SETFL, O_NONBLOCK");
		printf("errno = %d\n", errno);
	}
	else	{printf("\n\nfcntl F_SETFL, O_NONBLOCK: succeeded\n");}

#elif defined __sun__
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
	char err[50], msg[20];

	printf("\t%s: ", ptr->opt_str);

	if (ptr->opt_val_str == UNDEFINED)
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
				fprintf(stderr, "unknown %d level", ptr->opt_level);
				return;
			}

			sin_size = sizeof(val);
			if (getsockopt(sockfd, ptr->opt_level, ptr->opt_name, &val, &sin_size) == -1) {
				perror("getsockopt error");
			} else {
				switch (ptr->opt_val_str)	{
					case FLAG:
						snprintf(msg, sizeof(msg), "%s", sock_str_flag(&val, sin_size));
						break;
					case INTEGER:
						snprintf(msg, sizeof(msg), "%s", sock_str_int(&val, sin_size));
						break;
					case LINGER:
						snprintf(msg, sizeof(msg), "%s", sock_str_linger(&val, sin_size));
						break;
					case TIMEVAL:
						snprintf(msg, sizeof(msg), "%s", sock_str_timeval(&val, sin_size));
						break;
				}
				printf("default = %s\n", msg);
			}
		close(sockfd);
	}
	return;
}
