#ifndef SERVER_ACT_H
#define SERVER_ACT_H

#include "protocol.h"
#include "bool.h"

typedef enum
{
    RC_OK = 0,
    RC_ACCESS_DENIED,
    RC_NO_AUTH,
    RC_CMD_ERR,
    RC_BAD_CMD,
    RC_BAD_AUTH,
    RC_BAD_FILEDIR,
    RC_QUIT,
    RC_SOCKET_ERR,

    RC_COUNT
} rec_t;

typedef rec_t (*action_fn_t)(cmd_t * infos);



rec_t execute_command(cmd_t * c);

int send_error(cmd_t * infos, rec_t r);


rec_t action_list(cmd_t * infos);
rec_t action_pwd(cmd_t * infos);
rec_t action_retr(cmd_t * infos);
rec_t action_put(cmd_t * infos);
rec_t action_cwd(cmd_t * infos);
rec_t action_dele(cmd_t * infos);
rec_t action_mkdir(cmd_t * infos);
rec_t action_rmdir(cmd_t * infos);
rec_t action_help(cmd_t * infos);
rec_t action_quit(cmd_t * infos);
rec_t action_auth(cmd_t * infos);
rec_t action_error(cmd_t * infos);

rec_t create_dgram_channel(cmd_t * infos);

bool check_auth(cmd_t * infos);



#endif
