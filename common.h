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


#ifdef ENABLE_RELIABILITY
# define sendfile(fdfile, fd, to, tolen) sendfile_reliable(fdfile, fd, to, tolen)
# define recvfile(fdfile, fd, size, from, fromlen) recvfile_reliable(fdfile, fd, size, from, fromlen)
#else
# define sendfile(fdfile, fd, to, tolen) sendfile_raw(fdfile, fd, to, tolen)
# define recvfile(fdfile, fd, size, from, fromlen) recvfile_raw(fdfile, fd, size, from, fromlen)
#endif

/** send/recv sans fiabilite */
int sendfile_raw(int fdfile, int fd, struct sockaddr *to, socklen_t tolen);
int recvfile_raw(int fdfile, int fd, size_t filesize, struct sockaddr *from, socklen_t *fromlen);

/** send/recv sans fiabilite */
int sendfile_reliable(int fdfile, int fd, struct sockaddr *to, socklen_t tolen);
int recvfile_reliable(int fdfile, int fd, size_t filesize, struct sockaddr *from, socklen_t *fromlen);


int writeall(int fd, void * src, size_t s);
int readall(int fd, void * src, size_t s);

#define HANDLE_ERR(v, str) \
    do { if( (v) == -1 ) { perror(str); return EXIT_FAILURE; } } while(0)

#endif
