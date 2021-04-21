/**
 * @file sockopt.c
 * @author Adnan Omar (JUST ID: 123423)
 * @brief NES416-Assignment#3, get all sockets options and
 * 			change SO_SNDLOWAT and SO_RCVLOWAT values.
 * 		This file is for option value filtering for printing.
 * @date 2021-04-21
 * 
 */

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>


enum type {UNDEFINED, FLAG, INTEGER, LINGER, TIMEVAL};

union val {
  int				i_val;
  long				l_val;
  struct linger		linger_val;
  struct timeval	timeval_val;
} val;

static char	*sock_str_flag(union val *, int sin_size);
static char	*sock_str_int(union val *, int sin_size);
static char	*sock_str_linger(union val *, int sin_size);
static char	*sock_str_timeval(union val *, int sin_size);

static char	strres[128];

/**
 * @brief convert flag value to string after checking it, then and return it
 * 
 * @param val : value returned from getsockopt()
 * @param sin_size : size of val returned from getsockopt()
 * @return char* 
 */
static char	*sock_str_flag(union val *ptr, int sin_size) {
	if (sin_size != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", sin_size);
	else
		snprintf(strres, sizeof(strres), "%s", (ptr->i_val == 0) ? "off" : "on");
	return(strres);
}

/**
 * @brief convert int value to string after checking it, then and return it
 * 
 * @param val : value returned from getsockopt()
 * @param sin_size : size of val returned from getsockopt()
 * @return char* 
 */
static char	*sock_str_int(union val *ptr, int sin_size)  {
	if (sin_size != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", sin_size);
	else
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	return(strres);
}

/**
 * @brief convert linger value to string after checking it, then and return it
 * 
 * @param val : value returned from getsockopt()
 * @param sin_size : size of val returned from getsockopt()
 * @return char* 
 */
static char	*sock_str_linger(union val *ptr, int sin_size)   {
	struct linger	*lptr = &ptr->linger_val;

	if (sin_size != sizeof(struct linger))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)", sin_size);
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d", lptr->l_onoff, lptr->l_linger);
	return(strres);
}

/**
 * @brief convert timeval value to string after checking it, then and return it
 * 
 * @param val : value returned from getsockopt()
 * @param sin_size : size of val returned from getsockopt()
 * @return char* 
 */
static char	*sock_str_timeval(union val *ptr, int sin_size)  {
	struct timeval	*tvptr = &ptr->timeval_val;

	if (sin_size != sizeof(struct timeval))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct timeval)", sin_size);
	else
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec", tvptr->tv_sec, tvptr->tv_usec);
	return(strres);
}


