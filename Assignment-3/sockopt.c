#include "options.h"
#include <fcntl.h>

void get_option(struct sock_opts *ptr);

int main(int argc, char **argv)
{
	int	sockfd;
	struct sock_opts *ptr;

	/* loop on all options and test if they're available */
	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
		get_option(ptr);
	}

	int setsnd=10, setrcv=10, size, getsnd=0, getrcv=0;

	sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
/*
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK) < 0)	{
		perror("fcntl() failed with error: ");
	}
*/
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT, &setsnd, sizeof(int)) < 0)	{
		perror("setsockopt(SO_SNDLOWAT) failed with error : ");
	}

	*ptr = sock_opts[10];
	get_option(ptr);

	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT, &setrcv, sizeof(int)) < 0)	{
		perror("setsockopt(SO_SNDLOWAT) failed with error : ");
	}

	*ptr = sock_opts[9];
	get_option(ptr);

	close(sockfd);
	exit(0);
}


void get_option(struct sock_opts *ptr)	{
	int sockfd;
	socklen_t sin_size;
	char err[50];

	printf("%s: ", ptr->opt_str);

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
				sprintf(err, "Can't create sockfd for level: %d, opt_name: %s\n", ptr->opt_level, ptr->opt_str);
				perror(err);
		}

		sin_size = sizeof(val);
		if (getsockopt(sockfd, ptr->opt_level, ptr->opt_name, &val, &sin_size) == -1) {
			perror("getsockopt error");
		} else {
			printf("default = %s\n", (*ptr->opt_val_str)(&val, sin_size));
		}
		close(sockfd);
	}

}
