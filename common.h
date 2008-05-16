#ifndef COMMON_H
#define COMMON_H

#include "protocol.h"


/** Send all the 'size' bytes of 'buff' to 'fd' */
int sendtoall(int fd, char * buff, size_t size, struct sockaddr *to, socklen_t tolen);

/** Receive 'size' bytes from 'fd' to 'buff' */
int recvfromall(int fd, char * buff, size_t size, struct sockaddr *from, socklen_t *fromlen);


#define sendall(fd, buff, size) sendtoall(fd, buff, size, NULL, 0)
#define recvall(fd, buff, size) recvfromall(fd, buff, size, NULL, NULL)

int recvallline(int fd, char * dest, size_t s);

int sendfile(int fdfile, int fd, struct sockaddr *to, socklen_t tolen);
int revcfile(int fdfile, int fd, size_t filesize, struct sockaddr *from, socklen_t *fromlen);


int writeall(int fd, void * src, size_t s);
int readall(int fd, void * src, size_t s);

#define HANDLE_ERR(v, str) \
    do { if( (v) == -1 ) { perror(str); return EXIT_FAILURE; } } while(0)

#endif
