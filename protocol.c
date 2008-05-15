#include "protocol.h"
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
	       "ERROR"
	   };

cmd_type_t command_from_string(char * str)
{
    if(str == NULL)
	return C_ERROR;

    int i;

    for(i = 0; i < C_COUNT; ++i)
	if(!strcmp(str, cmd_names[i]))
	    return i;

    return C_ERROR;
}

char * command_to_string(cmd_type_t c)
{
    return cmd_names[c];
}


