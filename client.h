#ifndef CLIENT_H
#define CLIENT_H


#include "bool.h"
#include "common.h"

#define PROMPT "%eftp>: "
#define CLIENT_CMD_MAX_ARG 4
enum { CL_C_COUNT = 10 };


typedef struct
{
    int sockfd;
    bool quit;
} client_infos_t;

typedef void (*client_fn_t)(client_infos_t * infos);


int connect_to_server(char * server, port_t port);

void stop_client();

int wait_command(client_infos_t * infos);

void parse_command(client_infos_t * infos, char * cmd);

void print_error(char * str);

char * read_passphrase(char * buff, size_t size);

void flush_std();


void action_user(client_infos_t * infos);
void action_get(client_infos_t * infos);
void action_put(client_infos_t * infos);
void action_ls(client_infos_t * infos);
void action_cd(client_infos_t * infos);
void action_pwd(client_infos_t * infos);
void action_mkdir(client_infos_t * infos);
void action_rmdir(client_infos_t * infos);
void action_quit(client_infos_t * infos);
void action_rm(client_infos_t * infos);


#endif
