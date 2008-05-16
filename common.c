#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "bool.h"
#include "assert.h"

int sendtoall(int fd, char * buff, size_t size, struct sockaddr *to, socklen_t tolen)
{
    c_assert(buff);

    if(size == 0)
	return 0;

    size_t total = 0;
    size_t bytesleft = size;
    int n = -1;

    while(total < size)
    {
        n = sendto(fd, buff + total, bytesleft, MSG_NOSIGNAL, to, tolen);

        if (n == -1)
	    break;

        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
} 


int recvfromall(int fd, char * buff, size_t size, struct sockaddr *from, socklen_t *fromlen)
{
    c_assert(buff);

    size_t total = 0;
    size_t bytesleft = size;
    int n = -1;

    while(total < size)
    {
        n = recvfrom(fd, buff + total, bytesleft, MSG_NOSIGNAL, from, fromlen);

        if (n == -1)
	    break;

        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
} 

int writeall(int fd, void * src, size_t s)
{
    c_assert(s == 0 || (s && src));

    while(s)
    {
	int lu = write(fd, src, s);

	if(lu <= 0)
	    return lu;

	src += lu;
	s -= lu;
    }

    return 0;
}


int readall(int fd, void * dest, size_t s)
{
    while(s)
    {

	int lu = read(fd, dest, s);

	if(lu <= 0)
	    return lu;

	dest += lu;
	s -= lu;
    }

    return 1;
}

int recvallline(int fd, char * buff, size_t s)
{
    c_assert(buff);

    size_t total = 0;
    size_t bytesleft = s;
    int n = -1;

    while(total < s)
    {
        n = recv(fd, buff + total, bytesleft, MSG_NOSIGNAL);

        if (n == -1)
	    break;

	if(buff[total + n - 1] == '\n')
	{
	    buff[total + n - 1] = '\0';
	    return 0;
	}

        total += n;
        bytesleft -= n;
    }

    buff[s - 1] = '\0';

    return n == -1 ? -1 : 0;
}

int sendfile(int fdfile, int fd, struct sockaddr *to, socklen_t tolen)
{
    char buff[DEFAULT_BUFF_SIZE];

    int n;

    do
    {
	n = read(fdfile, buff, DEFAULT_BUFF_SIZE);
	if(n < 0)
	    return -2;
;
	if(sendtoall(fd, buff, n, to, tolen) < 0)
	    return -1;

    }
    while(n > 0);

    return 0;
}

int revcfile(int fdfile, int fd, size_t filesize, struct sockaddr *from, socklen_t *fromlen)
{
    char buff[DEFAULT_BUFF_SIZE];

    size_t tot = 0;

    do
    {
	int n = recvfrom(fd, buff, DEFAULT_BUFF_SIZE, MSG_NOSIGNAL, from, fromlen);
	if(n < 0)
	    return -1;

	if(writeall(fdfile, buff, n) < 0)
	    return -2;

	tot += n;

    }
    while(tot < filesize);

    return 0;
}





