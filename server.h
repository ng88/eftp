#ifndef SERVER_H
#define SERVER_H

#include "user.h"
#include "common.h"

#define SERVER_BACKLOG 5


int start_server(user_pool_t * existing_users, port_t port);

void stop_server();


int process_new_child(cmd_t * cmd);

int get_answer(cmd_t * cmd);
int send_answer(int fd, ans_t a, char code, char * message);


#endif
