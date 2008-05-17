#include "common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "misc.h"
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

#ifdef ENABLE_UDP_ERRORS
	/* en UDP seulement */
	int j = to ? (1 + (int) (20.0 * (rand() / (RAND_MAX + 1.0)))) : 0;
	switch(j)
	{
	case 20:
	    /* on perd un petit paquet 1 fois sur 20 */
	    n = min_u(bytesleft, (size_t)128);
	    c_warning2(false, "packet lost");
	    break;
	case 18:
	case 17:
	    /* on envoie simule des erreurs 2 fois sur 20 */
	    if(bytesleft > 0) buff[total] = buff[total] ^ 77;
	    if(bytesleft > 1) buff[total + 1] = buff[total + 1] ^ 45;
	    c_warning2(false, "packet corrupted");
	case 0:
	default:
	    /* on envoie normalement sinon */
	    n = sendto(fd, buff + total, bytesleft, MSG_NOSIGNAL, to, tolen);
	}
#else
	n = sendto(fd, buff + total, bytesleft, MSG_NOSIGNAL, to, tolen);
#endif        

        if (n == -1)
	    break;

        total += n;
        bytesleft -= n;
    }

    return n == -1 ? -1 : 0;
} 


int recvfromall(int fd, char * buff, size_t size, struct sockaddr *from, socklen_t fromlen)
{
    c_assert(buff);

    if(size == 0)
	return 0;

    size_t total = 0;
    size_t bytesleft = size;
    int n = -1;

    while(total < size)
    {
	socklen_t fl = fromlen;
        n = recvfrom(fd, buff + total, bytesleft, MSG_NOSIGNAL, from, &fl);

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

int sendfile_raw(int fdfile, int fd, struct sockaddr *to, socklen_t tolen)
{
    char buff[DEFAULT_BUFF_SIZE];

    int n;

    do
    {
	n = read(fdfile, buff, DEFAULT_BUFF_SIZE);
	if(n < 0)
	    return -2;

	if(sendtoall(fd, buff, n, to, tolen) < 0)
	    return -1;

    }
    while(n > 0);

    return 0;
}

int recvfile_raw(int fdfile, int fd, size_t filesize, struct sockaddr *from, socklen_t fromlen)
{
    char buff[DEFAULT_BUFF_SIZE];

    size_t tot = 0;

    do
    {
	socklen_t fl = fromlen;
	int n = recvfrom(fd, buff, DEFAULT_BUFF_SIZE, MSG_NOSIGNAL, from, &fl);
	if(n < 0)
	    return -1;

	if(writeall(fdfile, buff, n) < 0)
	    return -2;

	tot += n;

    }
    while(tot < filesize);

    return 0;
}


enum { HEADER_SIZE = sizeof(uint16_t) * 2 };

/** send/recv avec fiabilite */
int sendfile_reliable(int fdfile, int fd,
		      struct sockaddr *from,
		      struct sockaddr *to, socklen_t len)
{
    char buff[DEFAULT_BUFF_SIZE];
    uint16_t header[2];

    int n;

    do
    {
	n = read(fdfile, buff, DEFAULT_BUFF_SIZE);
	if(n < 0)
	    return -2;

	if(n == 0)
	    break;

	header[0] = htons((uint16_t)n);
	header[1] = htons((uint16_t)0);

	char ack = 'E';
	do
	{

	    if(sendtoall(fd, (char*)header, HEADER_SIZE, to, len) < 0)
		return -1;

	    if(sendtoall(fd, buff, n, to, len) < 0)
		return -1;

	    /* attend l'accusé */
	    if(recvfromall(fd, &ack, sizeof(ack), from, len) < 0)
		return -1;

	    dbg_printf("ack recu=%c\n", ack);

	}
	while(ack != 'O');

    }
    while(n > 0);

    return 0;
}

int recvfile_reliable(int fdfile, int fd, size_t filesize, 
		      struct sockaddr *from,
		      struct sockaddr *to, socklen_t len)
{
    char buff[DEFAULT_BUFF_SIZE];
    uint16_t header[2];

    dbg_printf("filesize=%u\n", filesize);

    size_t tot = 0;

    do
    {

	char ack = 'E';
	do
	{
	    int n = recvfromall(fd, (char*)header, HEADER_SIZE, from, len);
	    if(n < 0)
		return -1;

	    header[0] = ntohs(header[0]);
	    header[1] = ntohs(header[1]);

	    dbg_printf("header=%u %u  received=%u\n", header[0], header[1], tot);
	    
	    c_assert2(header[0] <= DEFAULT_BUFF_SIZE, "protocol violation");

	    if(header[0] == 0)
		return 0;

	    n = recvfromall(fd, buff, header[0], from, len);
	    if(n < 0)
		return -1;

	    ack = 'O';

	    dbg_printf("ack envye=%c\n", ack);

	    if(sendtoall(fd, &ack, sizeof(ack), to, len) < 0)
		return -1;
	}
	while(ack != 'O');

	if(writeall(fdfile, buff, header[0]) < 0)
	    return -2;

	tot += header[0];
	    
    }
    while(tot < filesize);

dbg_printf("done\n");

    return 0;
}






