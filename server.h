#ifndef SERVER_H
#define SERVER_H

#include "user.h"
#include "common.h"

#define SERVER_BACKLOG 5



int start_server(user_pool_t * existing_users, port_t port);

void stop_server();



#endif
