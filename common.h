#ifndef COMMON_H
#define COMMON_H

#include "protocol.h"
#include "md5.h"



/** Send all the 'size' bytes of 'buff' to 'fd' */
int sendall(int fd, char * buff, size_t size);

/** Receive 'size' bytes from 'fd' to 'buff' */
int recvall(int fd, char * buff, size_t size);

int recvallline(int fd, char * dest, size_t s);

char * create_challenge();

/** Compute the challenge answer */
void challenge_answer(char * challenge, char * userpwd, MD5_CTX_ppp * m);


int writeall(int fd, void * src, size_t s);
int readall(int fd, void * src, size_t s);

#define HANDLE_ERR(v, str) \
    do { if( (v) == -1 ) { perror(str); return EXIT_FAILURE; } } while(0)

#endif
