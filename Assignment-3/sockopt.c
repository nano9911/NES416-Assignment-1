/**
 * @file sockopt.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief NES416-Assignment#3, get all sockets options and
 * 			change SO_SNDLOWAT and SO_RCVLOWAT values.
 * 		This is the main file.
 * @date 2021-04-21
 * 
 */

#include "options.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>

#ifdef __sun__		/*headers only for Solaris*/
#include <stropts.h>
#include <sys/file.h>
#endif

static int socket_nonblock_option();
static int socket_nonblock_ioctl();
static int socket_nonblock_fcntl();

int main(int argc, char **argv)
{
	int	sockfd;
	socklen_t size=16;
	/* used to set SO_SNDLOWAT and SO_RCVLOWAT values */
	int setsnd=32, setrcv=128;
	/* used to compare SO_SNDLOWAT and SO_RCVLOWAT values
	   before setsokopt() and after*/
	int past_snd_val, past_rcv_val, cur_snd_val, cur_rcv_val;
	struct sock_opts *ptr;
	struct utsname buf;
	
	if (uname(&buf) == -1)	{
		perror("uname");
		exit(1);
	}

	printf("System info:\n%s %s %s %s %s\n\n", buf.sysname,
	    buf.nodename, buf.release, buf.version, buf.machine);

	/* loop on all options and test if they're available */
	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++)
		{get_option(ptr);}

	/*************************************************************/
	/* both options aren't defined in Solaris, So the test       */
	/* doesn't worth it.                                         */
#if defined __sun__
	printf("\nSolaris doesn't support SO_SNDLOWAT and SO_RCVLOWAT\n");

#elif (defined __linux__ || defined __FreeBSD__)
	/*************************************************************/
   	/* To create a non blocking AF_INET stream socket,           */
	/* we have two options:                                      */
	/* (Note: We are creating a Non-Blocking socket to make the  */
	/* change of SO_SNDLOWAT and SO_RCVLOWAT reasonable and safe)*/
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
	/* - comment out two and uncomment one to use the other one  */
   	/*************************************************************/
	/* getting user input:                                       */
	scanf("%d", &setsnd); scanf("%d", &setrcv);
   	/*************************************************************/

	/* getting current SO_SNDLOWAT and SO_RCVLOWAT values to     */
	/* compare after setsockopt()                                */
	if (getsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT,
	                    &past_snd_val, &size) == -1)
		{perror("\ngetsockopt SOL_SOCKET, SO_SNDLOWAT");}

	if (getsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT,
	                    &past_rcv_val, &size) == -1)
		{perror("\ngetsockopt SOL_SOCKET, SO_RCVLOWAT");}

	printf("\nPast values: SO_SNDLOWAT=%d, SO_RCVLOWAT=%d\n",
	                              past_snd_val ,past_rcv_val);
   	/*************************************************************/

	/* setting SO_SNDLOWAT to user chosed value                  */
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &setsnd,
	                                      sizeof(int)) == -1)
	{
		perror("\nsetsockopt SOL_SOCKET, SO_SNDLOWAT");
#ifdef __linux__
		printf("setsockopt() for SO_SNDLOWAT fails under linux\
	even it's defined, but it's unchangable.\n");
#endif
	}
	else	{
		printf("\nsetsockopt SOL_SOCKET, SO_SNDLOWAT: Succeeded.");
		if (getsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT,
                              &cur_snd_val, &size) == -1)
			{perror("\ngetsockopt SOL_SOCKET, SO_SNDLOWAT");}

		if (past_snd_val == cur_snd_val)
			{printf("But, value didn't change.\n");}
		printf("\nModified SO_SNDLOWAT value: %d\n", cur_snd_val);
	}
	/*************************************************************/

	/* setting SO_RCVLOWAT to user chosed value                  */
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &setrcv,
	                                       sizeof(int)) == -1)
		{perror("\nsetsockopt SOL_SOCKET, SO_RCVLOWAT");}
	else	{
		printf("\nsetsockopt SOL_SOCKET, SO_RCVLOWAT: Succeeded.");
		if (getsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT,
		                       &cur_rcv_val, &size) == -1)
			{perror("\ngetsockopt SOL_SOCKET, SO_RCVLOWAT");}

		if (past_rcv_val == cur_rcv_val)
			{printf(" But, value didn't change.\n");}
		printf("\nModified SO_RCVLOWAT value: %d\n", cur_rcv_val);
	}
	/*************************************************************/

	close(sockfd);

#endif /*defined __sun__ or (defined __linux__ || defined __FreeBSD__)*/

	exit(0);
}

/**
 * @brief a method to make a socket in nonblocking mode, by oring
 * SOCK_NONBLOCK with SOCK_STREAM in (type) argument of socket().
 * 
 * @return int : created socket descriptor
 */
int socket_nonblock_option()	{
	int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (sockfd == -1)	{
		perror("socket AF_INET, SOCK_STREAM | SOCK_NONBLOCK");
		fprintf(stderr, "Aboting, due to critical error\n");
		exit(0);
	}

	return sockfd;
}

/**
 * @brief a method to make a socket in nonblocking mode, using
 * ioctl() with setting option FIONBIO to 1in case of linux or freebsd,
 * or option _IONBF in case of solaris.
 * 
 * @return int : created socket descriptor
 */
int socket_nonblock_ioctl()	{
	int sockfd, on=1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
		perror("socket AF_INET, SOCK_STREAM");
		fprintf(stderr, "Aboting, due to critical error\n");
		exit(0);
	}

#if (defined __linux__ || defined __FreeBSD__)
	if (ioctl(sockfd, FIONBIO, (char *)&on) == -1)	{
		perror("ioctl FIONBIO, 1");
		fprintf(stderr, "Aboting, due to critical error\n");
	}
	else	{printf("\n\nioctl FIONBIO, 1: succeeded\n");}

#elif defined __sun__
	if (ioctl(sockfd, _IONBF, (char *)&on) == -1)	{
		perror("ioctl _IONBF, 1");
		fprintf(stderr, "Aboting, due to critical error\n");
	}
	else	{printf("\n\nioctl _IONBF, 1: succeeded\n");}

#endif /*(defined __linux__ || defined __FreeBSD__) or defined __sun__*/
	return sockfd;
}

/**
 * @brief a method to make a socket in nonblocking mode, using
 * fcntl() with option O_NONBLOCK in case of linux or freebsd,
 * or option FNDELAY in case of solaris.
 * 
 * @return int : created socket descriptor
 */
int socket_nonblock_fcntl()	{
	int sockfd, flags;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)	{
		perror("socket AF_INET, SOCK_STREAM");
		fprintf(stderr, "Aboting, due to critical error\n");
		exit(0);
	}

	flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1)	{
		perror("fcntl F_GETFL");
		fprintf(stderr, "Aboting, due to critical error\n");
	}

#if (defined __linux__ || defined __FreeBSD__)
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)	{
		perror("fcntl F_SETFL, O_NONBLOCK");
		fprintf(stderr, "Aboting, due to critical error\n");
	}
	else	{printf("\n\nfcntl F_SETFL, O_NONBLOCK: succeeded\n");}

#elif defined __sun__
	if (fcntl(sockfd, F_SETFL, flags | FNDELAY) == -1)	{
		perror("fcntl F_SETFL, FNDELAY");
		fprintf(stderr, "Aboting, due to critical error\n");
	}
	else 	{printf("\n\nfcntl F_SETFL, FNDELAY: succeeded\n");}

#endif /*(defined __linux__ || defined __FreeBSD__) or defined __sun__*/

	return sockfd;
}
