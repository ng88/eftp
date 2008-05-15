#ifndef SERVER_ACT_H
#define SERVER_ACT_H

#include "protocol.h"

typedef int (*action_fn_t)(cmd_t * infos);



int execute_command(cmd_t * c);



int action_list(cmd_t * infos);
int action_pwd(cmd_t * infos);
int action_retr(cmd_t * infos);
int action_put(cmd_t * infos);
int action_cwd(cmd_t * infos);
int action_dele(cmd_t * infos);
int action_mkdir(cmd_t * infos);
int action_rmdir(cmd_t * infos);
int action_help(cmd_t * infos);
int action_quit(cmd_t * infos);
int action_error(cmd_t * infos);



#endif
