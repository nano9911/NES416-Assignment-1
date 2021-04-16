#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

union val {
  int				i_val;
  long				l_val;
  struct linger		linger_val;
  struct timeval	timeval_val;
} val;

static char	*sock_str_flag(union val *, int);
static char	*sock_str_int(union val *, int);
static char	*sock_str_linger(union val *, int);
static char	*sock_str_timeval(union val *, int);

static char	strres[128];

static char	*sock_str_flag(union val *ptr, int sin_size) {
	if (sin_size != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", sin_size);
	else
		snprintf(strres, sizeof(strres), "%s", (ptr->i_val == 0) ? "off" : "on");
	return(strres);
}

static char	*sock_str_int(union val *ptr, int sin_size)  {
	if (sin_size != sizeof(int))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(int)", sin_size);
	else
		snprintf(strres, sizeof(strres), "%d", ptr->i_val);
	return(strres);
}

static char	*sock_str_linger(union val *ptr, int sin_size)   {
	struct linger	*lptr = &ptr->linger_val;

	if (sin_size != sizeof(struct linger))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct linger)", sin_size);
	else
		snprintf(strres, sizeof(strres), "l_onoff = %d, l_linger = %d", lptr->l_onoff, lptr->l_linger);
	return(strres);
}

static char	*sock_str_timeval(union val *ptr, int sin_size)  {
	struct timeval	*tvptr = &ptr->timeval_val;

	if (sin_size != sizeof(struct timeval))
		snprintf(strres, sizeof(strres), "size (%d) not sizeof(struct timeval)", sin_size);
	else
		snprintf(strres, sizeof(strres), "%ld sec, %ld usec", tvptr->tv_sec, tvptr->tv_usec);
	return(strres);
}
