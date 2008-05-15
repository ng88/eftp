#include "server_act.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
#include "user.h"
#include "vector.h"
#include "bool.h"

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


rec_t execute_command(cmd_t * c)
{
    c_assert(c);

    return (*(actions[c->type]))(c);
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
    c_warning2(false, "Not Yet Implemented " );
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
    return infos->user->login != NULL;
}
