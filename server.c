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

    user_t user;

    user.fd = fd;
    user.login = NULL;
    user.passphrase = NULL;

    cmd_t cmd;
    cmd.user = &user;

    if((ret = send_answer(fd, A_OK, 0, "waiting for user login and password")) < 0)
       return ret;

    while((ret = get_answer(&cmd)) > -1);

    close(fd);

    return ret;
}

int get_answer(cmd_t * cmd)
{
    c_assert(cmd && cmd->user);

    enum { BUFFS = 256 };

    char buff[BUFFS];

    if( (recvallline(cmd->user->fd, buff, BUFFS)) < -1)
	return -1;

    dbg_printf("rec command=%s\n", buff);

    command_from_string(buff, cmd);

    dbg_printf("cmd type=%d\n", cmd->type);

    if(cmd->type == C_ERROR)
	return send_answer(cmd->user->fd, A_ERROR, RC_BAD_CMD, "bad command or bad parameters");
    else
    {
	switch(execute_command(cmd))
	{
	case RC_ACCESS_DENIED:
	    send_answer(cmd->user->fd, A_ERROR, RC_ACCESS_DENIED, "access denied");
	    return 0;
	case RC_NO_AUTH:
	    send_answer(cmd->user->fd, A_ERROR, RC_NO_AUTH, "not logged in");
	    return 0;
	case RC_CMD_ERR:
	    send_answer(cmd->user->fd, A_ERROR, RC_CMD_ERR, "command error");
	    return 0;
	case RC_OK:
	    return 0;
	case RC_QUIT:
	default:
	    return -1;
	}
    }
}


int send_answer(int fd, ans_t a, char code, char * txt)
{
    enum { BUFFS = 12 + MAX_MSG_LEN };

    char buff[BUFFS];

    c_assert(txt == NULL || strlen(txt) < MAX_MSG_LEN);

    snprintf(buff, BUFFS, "%d%d %s %s\n",
	     a, code,
	     a == A_ERROR ? "error" : "ok",
	     txt ? txt : ""
	    );

    return sendall(fd, buff, strlen(buff));
}


