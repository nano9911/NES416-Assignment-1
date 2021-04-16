#include "options.h"
#include <fcntl.h>

#define var(which,var1,var2)  ((which) == (0) ? (var1) : (var2))

void get_option(struct sock_opts *ptr);

int main(int argc, char **argv)
{
	int	sockfd;
	struct sock_opts *ptr;

	/* loop on all options and test if they're available */
	for (ptr = sock_opts; ptr->opt_str != NULL; ptr++) {
		get_option(ptr);
	}

	char input_buf[10];
	int setsnd=10, setrcv=10, size, getsnd=0, getrcv=0;

/*	printf("Please enter new values for the receive low-water mark and the send low-water mark\
	 for TCP: ");

	fgets(input_buf, sizeof input_buf, stdin);

	for (int i = 0; i < strlen(input_buf); i++)	{
		if (input_buf[i] == ' ')	{sw+=1; continue;}
		if (input_buf[i] >= '0' && input_buf[i] <= '9')	{
			*(var(sw, &sndlowat, &rcvlowat)) += (int)input_buf[i]-48;
			*(var(sw, &sndlowat, &rcvlowat)) *= 10;
		}
	}
	if (sw != 1)	{
		printf("Invalid input, it should be like that: \n\txx yy\nwhere xx is SO_SNDLOWAT and yy is SO_RCVLOWAT\n\n");
		exit(0);
	}
	sndlowat /= 10; rcvlowat /= 10;
/*
	scanf("%d", sndlowat);
	scanf("%d", rcvlowat);*/
//	printf("user input: %d %d\n", sndlowat, rcvlowat);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL, 0) | O_NONBLOCK);

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