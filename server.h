#ifndef SERVER_H
#define SERVER_H

#include "user.h"
#include "common.h"

#define SERVER_BACKLOG 5


int start_server(user_pool_t * existing_users, port_t port);

void stop_server();


int process_new_child(int fd);


int send_answer(int fd, ans_t a, char * message);


#endif
