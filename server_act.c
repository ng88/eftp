#include "server_act.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assert.h"
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
	       &action_error,
	   };


int execute_command(cmd_t * c)
{
    c_assert(c);

    return (*(actions[c->type]))(c);

}

int action_list(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_pwd(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_retr(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_put(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_cwd(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_dele(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_mkdir(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_rmdir(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_help(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}

int action_quit(cmd_t * infos)
{
    return -2;
}

int action_error(cmd_t * infos)
{
    c_warning2(false, "Not Yet Implemented " );
    return 0;
}
