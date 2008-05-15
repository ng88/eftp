#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "bool.h"
#include "assert.h"

int sendall(int fd, char * buff, size_t size)
{
    c_assert(buff && size);

    size_t total = 0;
    size_t bytesleft = size;
    int n = -1;

    while(total < size)
    {
        n = send(fd, buff + total, bytesleft, MSG_NOSIGNAL);

        if (n == -1)
	    break;

        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
} 


int recvall(int fd, char * buff, size_t size)
{
    c_assert(buff);

    size_t total = 0;
    size_t bytesleft = size;
    int n = -1;

    while(total < size)
    {
        n = recv(fd, buff + total, bytesleft, MSG_NOSIGNAL);

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
    int i;

    while(total < s)
    {
        n = recv(fd, buff + total, bytesleft, MSG_NOSIGNAL);

        if (n == -1)
	    break;


	for(i = 0; i < n; ++i)
	    if(buff[total + i] == '\n')
	    {
		buff[total + i] = '\0';
		return 0;
	    }

        total += n;
        bytesleft -= n;
    }

    buff[s - 1] = '\0';

    return n == -1 ? -1 : 0;
}




