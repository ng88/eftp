#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <time.h>

#include "assert.h"
#include "vector.h"

#define RECV_BUFF_SIZE 768

static int server_run;
static user_pool_t * existing_users;
static vector_t * users; /* connected users */
static FILE * logfile;

int start_server(user_pool_t * eu, port_t port)
{
    server_run = 1;
    existing_users = eu;

    struct sockaddr_in myaddr;
    struct sockaddr_in rmaddr;

    int fdmax;
    int fdlisten;

    size_t s;
    size_t i;

    int yes = 1;

    if((fdlisten = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    if(setsockopt(fdlisten, SOL_SOCKET, SO_REUSEADDR,
		  &yes, sizeof(yes)) == -1)
    {
        perror("setsockopt");
        return EXIT_FAILURE;
    }

    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(port);
    memset(myaddr.sin_zero, 0, sizeof(myaddr.sin_zero));

    if(bind(fdlisten, (struct sockaddr *)&myaddr, sizeof(myaddr)) == -1)
    {
        perror("bind");
        return EXIT_FAILURE;
    }


    if(listen(fdlisten, SERVER_BACKLOG) == -1)
    {
        perror("listen");
	return EXIT_FAILURE;
    }

    dbg_printf("server started\n");

    while(server_run)
    {

	socklen_t addrlen = sizeof(rmaddr);
	    
	int fd = accept(fdlisten,
			(struct sockaddr *)&rmaddr,
			&addrlen);
	    
	if(fd == -1)
	    perror("accept");
	else
	{

	    dbg_printf("incoming connection from %s on socket %d\n",
		       inet_ntoa(rmaddr.sin_addr), fd);

	    /*
	      fork ici
	    */
	    
	}
	    
	

    }

    close(fdlisten);
    dbg_printf("server halted\n");
    
    return EXIT_SUCCESS;
}



void stop_server()
{
    server_run = 0;
}


