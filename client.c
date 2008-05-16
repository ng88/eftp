#define _GNU_SOURCE
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <termios.h>
#include <stdarg.h>

#define _XOPEN_SOURCE_
#include <stdlib.h>

#include "client.h"
#include "assert.h"
#include "misc.h"

#define RECV_BUFF_SIZE DEFAULT_BUFF_SIZE

static char * cmd_names[CL_C_COUNT] =
           {
	       "user",
	       "get",
	       "put",
	       "ls",
	       "cd",
	       "pwd",
	       "mkdir",
	       "rmdir",
	       "quit",
	       "rm",
	   };

static char cmd_arg_count[CL_C_COUNT] =
           {
	       1,
	       1,
	       1,
	       0,
	       1,
	       0,
	       1,
	       1,
	       0,
	       1,
	   };

static client_fn_t cmd_actions[C_COUNT] =
           {
	       &action_user,
	       &action_get,
	       &action_put,
	       &action_ls,
	       &action_cd,
	       &action_pwd,
	       &action_mkdir,
	       &action_rmdir,
	       &action_quit,
	       &action_rm,
	   };

static bool run;

int connect_to_server(char * server, port_t port)
{
    c_assert(server);

    int sockfd;
    struct sockaddr_in dest_addr;

    HANDLE_ERR(sockfd = socket(PF_INET, SOCK_STREAM, 0), "socket");

    struct hostent * h;

    if((h = gethostbyname(server)) == NULL)
    {
        herror("gethostbyname");
        return EXIT_FAILURE;
    }

    dbg_printf("trying to connect to %s (%s:%u)...\n", server, 
	       inet_ntoa(*((struct in_addr *)h->h_addr)), port);


    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr = *((struct in_addr *)h->h_addr);
    memset(dest_addr.sin_zero, 0, sizeof(dest_addr.sin_zero));


    HANDLE_ERR(connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)), "connect");

    dbg_printf("connected.\n");

    /*     SEND LOGIN    */
/*    int n = min_u(strlen(login) + 1, (size_t)USER_MAX_LOGIN_SIZE);
    if(sendall(sockfd, login, &n) == -1)
    {
	perror("login");
	close(sockfd);
	return EXIT_FAILURE;
    }
*/

    char buff[DEFAULT_BUFF_SIZE];
    if(recvallline(sockfd, buff, DEFAULT_BUFF_SIZE))
	return EXIT_FAILURE;

    client_infos_t infos;
    infos.sockfd = sockfd;
    infos.quit = false;
    wait_command(&infos);

    close(sockfd);
    dbg_printf("client halted.\n");

    return EXIT_SUCCESS;
}

void stop_client()
{
    run = false;
}

int wait_command(client_infos_t * infos)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fputs(PROMPT, stdout);

    while((read = getline(&line, &len, stdin)) != -1)
    {
	if(read > 1)
	{
	    line[read - 1] = '\0';
	    parse_command(infos, line);

	    if(infos->quit)
		break;
	}
	fputs(PROMPT, stdout);
    }

    if (line)
	free(line);

    return EXIT_SUCCESS;	   
}

void parse_command(client_infos_t * infos, char * str)
{
    c_assert(str);

    int i;
    char * cmd = strtok(str, " ");

    if(!cmd)
    {
	print_error("syntax error");
	return;
    }

    char * ptr = cmd;

    while(*ptr)
    {
	*ptr = tolower(*ptr);
	ptr++;
    }

    int icmd = -1;

    for(i = 0; i < CL_C_COUNT; ++i)
	if(!strcmp(cmd_names[i], cmd))
	{
	    icmd = i;
	    break;
	}

    if(icmd == -1)
    {
	print_error("invalid command");
	return;
    }

    for(i = 0; i < CLIENT_CMD_MAX_ARG; ++i)
    {
	infos->args[i] = strtok(NULL, " ");

	if(!infos->args[i])
	    break;
    }

    if(i != cmd_arg_count[icmd])
    {
	print_error("bad number of argument");
	return;
    }
    infos->quit = false;
    infos->list = (cmd_actions[icmd] == action_ls);
    (*(cmd_actions[icmd]))(infos);

}


void print_error(char * str)
{
    fprintf(stderr, "error: %s\n", str);
}


char * read_passphrase(char * buff, size_t size)
{
    c_assert(buff);

    fputs("Enter passphrase: ", stdout);
    fflush(stdout);

    char * ret = buff;

    struct termios settings;

    if( tcgetattr(1, &settings) == -1 )
    {
	perror("tcgetattr");
	return NULL;
    }

    settings.c_lflag &= ~ECHO;

    if( tcsetattr(1, TCSANOW, &settings) == -1 )
    {
	perror("tcsetattr");
	return NULL;
    }

    if(fgets(buff, size, stdin))
    {
	buff[size - 1] = '\0';
	char * p = strchr(buff, '\n');
	if(p) *p = '\0';
    }
    else
	ret = NULL;

    settings.c_lflag |= ECHO;

    if( tcsetattr(1, TCSANOW, &settings) == -1 )
    {
	perror("tcsetattr");
	return NULL;
    }
    
    putchar('\n');

    return ret;
}



void flush_std()
{
    fflush(stdout);
    fflush(stderr);

/*  
    ungetc('*', stdin); 
    int ch;
    while( (ch = getc(stdin)) != EOF && ch != '\n');
*/

}

char * send_command(client_infos_t * infos, bool print, char * cmd, ...)
{
    static char buff[DEFAULT_BUFF_SIZE];

    infos->success = false;

    va_list(ap);
    va_start(ap, cmd);
    vsnprintf(buff, DEFAULT_BUFF_SIZE, cmd, ap);
    va_end(ap);

    buff[DEFAULT_BUFF_SIZE - 1] = '\0';

    int ret = sendall(infos->sockfd, buff, strlen(buff));
    if(ret < 0)
    {
	infos->quit = false;
	return NULL;
    }

    ret = recvallline(infos->sockfd, buff, DEFAULT_BUFF_SIZE);
    if(ret < 0)
    {
	infos->quit = false;
	return NULL;
    }

    //dbg_printf("buffer=%s\n", buff);

    infos->success = (buff[0] != (A_ERROR +'0'));

    if(print || !infos->success)
    {
	buff[0] = '>';
	buff[1] = ' ';
	printf("%s\n", buff);
    }

    if(infos->list && infos->success)
    {
	char * pos;
	do
	{
	    ret = recvallline(infos->sockfd, buff, DEFAULT_BUFF_SIZE);
	    if(ret < 0)
	    {
		infos->quit = false;
		return NULL;
	    }
	    //dbg_printf("buffer=%s\n", buff);

	    pos = strstr(buff, "10 ok");

	    if(pos)
	    {
		if(pos != buff)
		    pos--;

		*pos = '\0';
	    }

	    if(pos != buff)
		printf("%s\n", buff);

	}
	while(!pos);
    }

    return buff;

}


void action_user(client_infos_t * infos)
{
    char buff[DEFAULT_BUFF_SIZE];
    if(read_passphrase(buff, DEFAULT_BUFF_SIZE))
	send_command(infos, false, "AUTH %s %s\n", infos->args[0], buff);
}

void action_ls(client_infos_t * infos)
{
    send_command(infos, false, "LIST .\n");
}

void action_cd(client_infos_t * infos)
{
    send_command(infos, true, "CWD %s\n", infos->args[0]);
}

void action_pwd(client_infos_t * infos)
{
    send_command(infos, true, "PWD\n");
}

void action_mkdir(client_infos_t * infos)
{
    send_command(infos, false, "MKDIR %s\n", infos->args[0]);
}

void action_rmdir(client_infos_t * infos)
{
    send_command(infos, false, "RMDIR %s\n", infos->args[0]);
}

void action_quit(client_infos_t * infos)
{
    send_command(infos, false, "QUIT\n");
    infos->quit = true;
}

void action_rm(client_infos_t * infos)
{
    send_command(infos, false, "DELE %s\n", infos->args[0]);
}

void action_get(client_infos_t * infos)
{
}

void action_put(client_infos_t * infos)
{
    int file = open(infos->args[0], DEFAULT_READ_FILE_FLAGS);
    if(file < 0)
    {
	print_error("unable to put file");
	return;
    }

    struct stat finfos;
    if(fstat(file, &finfos) < 0)
    {
	print_error("unable to stat file");
	return;
    }

    char * ret;
    if((ret = send_command(infos, false, "PUT %s %u\n",
			  infos->args[0], (size_t)finfos.st_size)))
    {
	int port;
	sscanf(ret, "30 port (%d)", &port);


	struct sockaddr_in myaddr, si_other;
	int datafd;

	if((datafd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
	    print_error("socket error");
	    close(file);
	    return;
	}

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(port);
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(myaddr.sin_zero, 0, sizeof(myaddr.sin_zero));
	

	int n = sendfile(file, datafd, (struct sockaddr *)&myaddr, sizeof(myaddr));

	close(file);
	close(datafd);
	
	if(n == -2)
	    print_error("file error");
	else if(n == -1)
	    print_error("socket error");
    }
    else
	close(file);
}
