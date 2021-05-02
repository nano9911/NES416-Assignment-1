/**
 * @file sockopt.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief NES416-Assignment#3, get all sockets options and 
 * 			change SO_SNDLOWAT and SO_RCVLOWAT values.
 * 		This file contains all options.
 * @date 2021-04-21
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>		/* for TCP_xxx defines */
#include "option_filtering.h"


struct sock_opts {
  const char *opt_str;
  int opt_level;
  int opt_name;
  int opt_val_str;
//  char *(*opt_val_str)(union val *, int);
} sock_opts[] = {
	/* SOL_SOCKET options */
    { "SO_BROADCAST",       SOL_SOCKET,	SO_BROADCAST,   FLAG },
	{ "SO_DEBUG",           SOL_SOCKET,	SO_DEBUG,       FLAG },
	{ "SO_DONTROUTE",       SOL_SOCKET,	SO_DONTROUTE,   FLAG },
	{ "SO_ERROR",           SOL_SOCKET,	SO_ERROR,        INTEGER },
	{ "SO_KEEPALIVE",       SOL_SOCKET,	SO_KEEPALIVE,   FLAG },
	{ "SO_LINGER",          SOL_SOCKET,	SO_LINGER,    LINGER },
	{ "SO_OOBINLINE",       SOL_SOCKET,	SO_OOBINLINE,   FLAG },
	{ "SO_RCVBUF",          SOL_SOCKET,	SO_RCVBUF,       INTEGER },
	{ "SO_SNDBUF",          SOL_SOCKET,	SO_SNDBUF,       INTEGER },
#if (defined SO_RCVLOWAT && !defined __sun__)
	{ "SO_RCVLOWAT",        SOL_SOCKET,	SO_RCVLOWAT,     INTEGER },
#else
	{ "SO_RCVLOWAT",        0,0,UNDEFINED },
#endif /*(defined SO_RCVLOWAT && !defined __sun__)*/
#if (defined SO_SNDLOWAT && !defined __sun__)
	{ "SO_SNDLOWAT",        SOL_SOCKET,	SO_SNDLOWAT,     INTEGER },
#else
	{ "SO_SNDLOWAT",        0,0,UNDEFINED },
#endif /*(defined SO_SNDLOWAT && !defined __sun__)*/
	{ "SO_RCVTIMEO",        SOL_SOCKET,	SO_RCVTIMEO, TIMEVAL },
	{ "SO_SNDTIMEO",        SOL_SOCKET,	SO_SNDTIMEO, TIMEVAL },
	{ "SO_REUSEADDR",       SOL_SOCKET,	SO_REUSEADDR,   FLAG },
#ifdef	SO_REUSEPORT
	{ "SO_REUSEPORT",       SOL_SOCKET,	SO_REUSEPORT,   FLAG },
#else
	{ "SO_REUSEPORT",       0,0,UNDEFINED },
#endif /*SO_REUSEPORT*/
	{ "SO_TYPE",           SOL_SOCKET,  SO_TYPE,         INTEGER },
#ifdef SO_USELOOPBACK
	{ "SO_USELOOPBACK",    SOL_SOCKET,	SO_USELOOPBACK,	FLAG },
#else
	{ "SO_USELOOPBACK",    0,0,UNDEFINED },
#endif /*SO_USELOOPBACK*/

	/* IPPROTO_IP options */
	{ "IP_TOS",            IPPROTO_IP,	IP_TOS,         INTEGER },
	{ "IP_TTL",            IPPROTO_IP,	IP_TTL,         INTEGER },


	/* IPPROTO_IPV6 options */
#ifdef	IPPROTO_IPV6
	#ifdef	IPV6_DONTFRAG
	{ "IPV6_DONTFRAG",     IPPROTO_IPV6,IPV6_DONTFRAG,  FLAG },
	#else
	{ "IPV6_DONTFRAG",		0,0,UNDEFINED },
	#endif /*IPV6_DONTFRAG*/
	#ifdef	IPV6_UNICAST_HOPS
	{ "IPV6_UNICAST_HOPS", IPPROTO_IPV6,IPV6_UNICAST_HOPS,INTEGER},
	#else
	{ "IPV6_UNICAST_HOPS",  0,0,UNDEFINED },
	#endif /*IPV6_UNICAST_HOPS*/
	#ifdef	IPV6_V6ONLY
	{ "IPV6_V6ONLY",        IPPROTO_IPV6,IPV6_V6ONLY,   FLAG },
	#else
	{ "IPV6_V6ONLY",        0,0,UNDEFINED },
	#endif /*IPV6_V6ONLY*/
#endif /*IPPROTO_IPV6*/

	/* IPPROTO_TCP options */
	{ "TCP_MAXSEG",         IPPROTO_TCP,TCP_MAXSEG,     INTEGER },
	{ "TCP_NODELAY",        IPPROTO_TCP,TCP_NODELAY,   FLAG },


	/* IPPROTO_SCTP options */
#ifdef IPPROTO_SCTP
	#ifdef	SCTP_AUTOCLOSE
	{ "SCTP_AUTOCLOSE",     IPPROTO_SCTP,SCTP_AUTOCLOSE,INTEGER },
	#else
	{ "SCTP_AUTOCLOSE",     0,0,UNDEFINED },
	#endif /*SCTP_AUTOCLOSE*/
	#ifdef	SCTP_MAXBURST
	{ "SCTP_MAXBURST",      IPPROTO_SCTP,SCTP_MAXBURST, INTEGER },
	#else
	{ "SCTP_MAXBURST",      0,0,UNDEFINED },
	#endif /*SCTP_MAXBURST*/
	#ifdef	SCTP_MAXSEG
	{ "SCTP_MAXSEG",        IPPROTO_SCTP,SCTP_MAXSEG,   INTEGER },
	#else
	{ "SCTP_MAXSEG",        0,0,UNDEFINED },
	#endif /*SCTP_MAXSEG*/
	#ifdef	SCTP_NODELAY
	{ "SCTP_NODELAY",       IPPROTO_SCTP,SCTP_NODELAY, FLAG },
	#else
	{ "SCTP_NODELAY",       0,0,UNDEFINED },
	#endif /*IPPROTO_SCTP*/
	{ NULL,                 0,0,UNDEFINED }
#endif /*IPPROTO_SCTP*/
};

/**
 * @brief Get and print the option name, value or state.
 * 
 * @param ptr a pointer to a sock_opts structure.
 */
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
				sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
				break;
		#ifdef	IPPROTO_IPV6
			case IPPROTO_IPV6:
				sockfd = socket(AF_INET6, SOCK_STREAM, 0);
				break;
		#endif /*IPPROTO_IPV6*/
		#ifdef	IPPROTO_SCTP
			case IPPROTO_SCTP:
				sockfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
				break;
		#endif /*IPPROTO_SCTP*/
			default:
				fprintf(stderr, "unknown %d level", ptr->opt_level);
				return;
			}

			sin_size = sizeof(val);
			if (getsockopt(sockfd, ptr->opt_level, ptr->opt_name, &val, &sin_size) == -1) {
				perror("getsockopt error");
			}

			else {
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
