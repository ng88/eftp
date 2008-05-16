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
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#include "assert.h"
#include "vector.h"
#include "server_act.h"

#define RECV_BUFF_SIZE DEFAULT_BUFF_SIZE


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
	printf("avt\n");
	int fd = accept(fdlisten,
			(struct sockaddr *)&rmaddr,
			&addrlen);
	printf("apr\n");
	if(fd == -1)
	{
	    if(errno != EINTR)
		perror("accept");
	}
	else
	{

	    dbg_printf("incoming connection from %s on socket %d\n",
		       inet_ntoa(rmaddr.sin_addr), fd);

	    pid_t c = fork();

	    if(c == 0)
	    {
	     /* fils */
		close(fdlisten);
		process_new_child(fd);
		dbg_printf("child halted\n");
		return EXIT_SUCCESS;
	    }
	    else if(c == -1)
		c_warning2(0, "fork error");
	    else
	    { /* pere */
		close(fd);
	    }
	    
	}

    }

    close(fdlisten);
    dbg_printf("server halted\n");
    
    return EXIT_SUCCESS;
}



void stop_server()
{
    static int count = 0;
    count++;
    if(count > 1) return;

    server_run = 0;
    kill(0, SIGTERM);
    while(wait(NULL) != -1);
}


int process_new_child(int fd)
{
    int ret = 0;

    cmd_t cmd;
    cmd.user = NULL;
    cmd.pool = existing_users;
    cmd.fd = fd;
    cmd.datafd = -1;

    if((ret = send_answer(fd, A_OK, 0, "waiting for user login and password")) < 0)
       return ret;

    while((ret = get_answer(&cmd)) > -1);

    close(fd);

    return ret;
}

int get_answer(cmd_t * cmd)
{
    c_assert(cmd);

    char buff[DEFAULT_BUFF_SIZE];

    if( (recvallline(cmd->fd, buff, DEFAULT_BUFF_SIZE)) < -1)
	return -1;

    dbg_printf("rec command=%s\n", buff);

    command_from_string(buff, cmd);

    dbg_printf("cmd type=%d\n", cmd->type);

    if(cmd->type == C_ERROR)
	return send_error(cmd, RC_BAD_CMD);
    else
    {
	rec_t v = execute_command(cmd);

	switch(v)
	{
	case RC_OK:
	    return 0;
	case RC_SOCKET_ERR:
	case RC_QUIT:
	    return -1;
	default:
	    return send_error(cmd, v);
	}
    }
}


int send_answer(int fd, ans_t a, char code, char * txt)
{
    enum { BUFFS = 24 + MAX_MSG_LEN };

    char buff[BUFFS];

    c_assert(txt == NULL || strlen(txt) < MAX_MSG_LEN);

    char * ma;

    switch(a)
    {
    case A_ERROR: ma = "error"; break;
    case A_OK: ma = "ok"; break;
    case A_OK_DATA_FOLLOW: ma = "ok data follow"; break;
    case A_OK_PORT: ma = "port"; break;
    default: ma = "???"; break;
    }

    snprintf(buff, BUFFS, "%d%d %s %s\n",
	     a, code,
	     ma,
	     txt ? txt : ""
	    );

    return sendall(fd, buff, strlen(buff));
}


