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
    char * args[CLIENT_CMD_MAX_ARG];
    char * cmd = strtok(str, " ");

    if(!cmd)
    {
	print_error("syntax error");
	return;
    }

    char * ptr = cmd;
    while(*cmd)
    {
	*cmd = tolower(*cmd);
	cmd++;
    }
    printf(">>>%s\n", cmd);

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
	args[i] = strtok(NULL, " ");

	if(!args[i])
	    break;
    }

    if(i != cmd_arg_count[icmd])
    {
	print_error("bad number of argument");
	return;
    }
    printf("%d %p\n", icmd, cmd_actions[icmd]);
    infos->quit = false;
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


void action_user(client_infos_t * infos)
{
}

void action_get(client_infos_t * infos)
{
}

void action_put(client_infos_t * infos)
{
}

void action_ls(client_infos_t * infos)
{
}

void action_cd(client_infos_t * infos)
{
}

void action_pwd(client_infos_t * infos)
{
}

void action_mkdir(client_infos_t * infos)
{
}

void action_rmdir(client_infos_t * infos)
{
}

void action_quit(client_infos_t * infos)
{
    infos->quit = true;
}

void action_rm(client_infos_t * infos)
{
}

