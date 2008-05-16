#include "server_act.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include "assert.h"
#include "user.h"
#include "vector.h"
#include "misc.h"
#include "server.h"

static action_fn_t actions[C_COUNT] =
           {
	       &action_list,
	       &action_pwd,
	       &action_retr,
	       &action_put,
	       &action_cwd,
	       &action_dele,
	       &action_mkdir,
	       &action_rmdir,
	       &action_help,
	       &action_quit,
	       &action_auth,
	       &action_error,
	   };

static char * error_msg[RC_COUNT] = 
           {
	       "",
	       "access denied",
	       "not logged in",
	       "command error",
	       "bad command or bad parameters",
	       "bad login or password",
	       "file or directory does not exist",
	       "",
	       "",
	   };

rec_t execute_command(cmd_t * c)
{
    c_assert(c);

    return (*(actions[c->type]))(c);
}

int send_error(cmd_t * infos, rec_t r)
{
    return send_answer(infos->fd, A_ERROR, r, error_msg[r]);
}


bool check_auth(cmd_t * infos)
{
    c_assert(infos);
    //return infos->user != NULL;
    return true; // desactive l'auth pr debug plus vite
}

rec_t action_list(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    DIR * d;
    struct dirent * dinfos;
    d = opendir(infos->args[0]);

    if(!d)
	return RC_BAD_FILEDIR;

    if(send_answer(infos->fd, A_OK_DATA_FOLLOW, 0, NULL) < 0)
	return RC_SOCKET_ERR;

    size_t pos = 0;
    char buff[NAME_MAX + 1];

    while((dinfos = readdir(d)))
    {

	size_t len = strlen(dinfos->d_name);
	memcpy(buff, dinfos->d_name, len);
	buff[len] = '\n';

	if(sendall(infos->fd, buff, len + 1) < 0)
	    return RC_SOCKET_ERR;
	
    }

    if(send_answer(infos->fd, A_OK, 0, NULL) < 0)
	return RC_SOCKET_ERR;
    
    closedir(d);

    return RC_OK;
}

rec_t action_pwd(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    char buff[PATH_MAX];

    if(!getcwd(buff, PATH_MAX))
	return RC_BAD_FILEDIR;

    if(send_answer(infos->fd, A_OK, 0, buff) < 0)
	return RC_SOCKET_ERR;

    return RC_OK;
}

rec_t action_cwd(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    if(chdir(infos->args[0]) < 0)
	return RC_BAD_FILEDIR;

    return action_pwd(infos);
}

rec_t action_mkdir(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    if(mkdir(infos->args[0], 0750) < 0)
	return errno == EACCES ? RC_ACCESS_DENIED : RC_BAD_FILEDIR;

    if(send_answer(infos->fd, A_OK, 0, NULL) < 0)
	return RC_SOCKET_ERR;

    return RC_OK;
}

rec_t action_rmdir(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    if(rmdir(infos->args[0]) < 0)
	return errno == EACCES ? RC_ACCESS_DENIED : RC_BAD_FILEDIR;

    if(send_answer(infos->fd, A_OK, 0, NULL) < 0)
	return RC_SOCKET_ERR;

    return RC_OK;
}

rec_t action_dele(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    if(unlink(infos->args[0]) < 0)
	return errno == EACCES ? RC_ACCESS_DENIED : RC_BAD_FILEDIR;

    if(send_answer(infos->fd, A_OK, 0, NULL) < 0)
	return RC_SOCKET_ERR;

    return RC_OK;
}

rec_t action_help(cmd_t * infos)
{
    int i;

    enum { BUFFS= 32 };
    char buff[BUFFS];

    if(send_answer(infos->fd, A_OK_DATA_FOLLOW, 0, "following commands are available") < 0)
	return RC_SOCKET_ERR;

    for(i = 0; i < C_ERROR; ++i)
    {
	char ac = command_arg_count(i);

	snprintf(buff, BUFFS, "%s (%d parameter%s)\n",
		command_type_to_string(i),
		ac,
		ac > 1 ? "s" : ""
	    );

	if(sendall(infos->fd, buff, strlen(buff)) < 0)
	    return RC_SOCKET_ERR;


    }

    if(send_answer(infos->fd, A_OK, 0, NULL) < 0)
	return RC_SOCKET_ERR;

    return RC_OK;
}

rec_t action_quit(cmd_t * infos)
{
    return RC_QUIT;
}

rec_t action_auth(cmd_t * infos)
{
    printf("%s:%s\n", infos->args[0], infos->args[1]);

    user_t * user = get_user_from_name(infos->pool, infos->args[0]);

    if(!user)
	return RC_BAD_AUTH;

    if(!check_user_passphrase(user, infos->args[1]))
	return RC_BAD_AUTH;

    infos->user = user;

    if(send_answer(infos->fd, A_OK, 0, "welcome") < 0)
	return RC_SOCKET_ERR;

    return RC_OK;
}

rec_t action_error(cmd_t * infos)
{
    return RC_BAD_CMD;
}



rec_t action_retr(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;


    return RC_OK;
}

rec_t action_put(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    rec_t r;

    struct sockaddr_in myaddr, si_other;

    if((infos->datafd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	return RC_SOCKET_ERR;

    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(SERVER_DEFAULT_DATA_PORT);
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(myaddr.sin_zero, 0, sizeof(myaddr.sin_zero));

    if(bind(infos->datafd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0)
    {
	close(infos->datafd);
	return RC_SOCKET_ERR;
    }
    
    char buff[MAX_MSG_LEN];

    snprintf(buff, MAX_MSG_LEN, "(%d)", ntohs(myaddr.sin_port));

    if(send_answer(infos->fd, A_OK_PORT, 0, buff) < 0)
    {
	close(infos->datafd);
	return RC_SOCKET_ERR;
    }


    socklen_t fromlen = sizeof(myaddr);

    int file = open(infos->args[0], DEFAULT_WRITE_FILE_FLAGS, 0666);

    if(file < 0)
    {
	close(infos->datafd);
	return errno == EACCES ? RC_ACCESS_DENIED : RC_BAD_FILEDIR;
    }

    int n = revcfile(file, infos->datafd, atoi(infos->args[1]), (struct sockaddr *)&myaddr, &fromlen);

    close(file);
    close(infos->datafd);

    if(n == -2)
	return RC_BAD_FILEDIR;
    else if(n == -1)
	return RC_SOCKET_ERR;
    else
	return RC_OK;

}




