
#include "assert.h"
#include "protocol.h"
#include "user.h"
#include <string.h>

static char * cmd_names[C_COUNT] =
           {
	       "LIST",
	       "PWD",
	       "RETR",
	       "PUT",
	       "CWD",
	       "DELE",
	       "MKDIR",
	       "RMDIR",
	       "HELP",
	       "QUIT",
	       "AUTH",
	       "ERROR"
	   };

static char cmd_arg_count[C_COUNT] =
           {
	       1,
	       0,
	       1,
	       2,
	       1,
	       1,
	       1,
	       1,
	       0,
	       0,
	       2,
	       0,
	   };

cmd_type_t command_type_from_string(char * str)
{
    if(!str)
	return C_ERROR;

    int i;

    for(i = 0; i < C_COUNT; ++i)
  	if(!strcmp(str, cmd_names[i]))
	    return i;
  
  return C_ERROR;
}

char * command_type_to_string(cmd_type_t c)
{
    return cmd_names[c];
}

char command_arg_count(cmd_type_t c)
{
    c_assert(cmd_arg_count[c] < CMD_MAX_ARG);
    return cmd_arg_count[c];
}


void command_from_string(char * str, cmd_t * dest)
{
    c_assert(str && dest);

    int i;

    char * cmd = strtok(str, " ");

    if(!cmd)
    {
	dest->type = C_ERROR;
	return;
    }

    dest->type = command_type_from_string(cmd);
    if(dest->type == C_ERROR)
	return;

    for(i = 0; i < CMD_MAX_ARG; ++i)
    {
	dest->args[i] = strtok(NULL, " ");

	if(!dest->args[i])
	    break;

    }

    if(i != command_arg_count(dest->type))
	dest->type = C_ERROR;
}


