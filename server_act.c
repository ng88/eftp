#include "server_act.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "user.h"
#include "vector.h"
#include "bool.h"
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




rec_t action_list(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_pwd(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_retr(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_put(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_cwd(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_dele(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_mkdir(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_rmdir(cmd_t * infos)
{
    if(!check_auth(infos))
	return RC_NO_AUTH;

    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

rec_t action_help(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
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

    send_answer(infos->fd, A_OK, 0, "welcome");

    return RC_OK;
}

rec_t action_error(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return RC_OK;
}

bool check_auth(cmd_t * infos)
{
    c_assert(infos);
    return infos->user != NULL;
}
