#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <netinet/in.h>
typedef uint16_t port_t;

#define SERVER_DEFAULT_PORT 2021

#define USER_MAX_PASS_SIZE 64
#define USER_MAX_LOGIN_SIZE 64

#define CHALLENGE_SIZE 16


typedef enum
{
    A_OK = 1,
    A_OK_DATA_FOLLOW = 2,
    A_ERROR = 3,
} ans_t;

#define MAX_MSG_LEN 64

typedef enum
{
    C_LIST = 0,
    C_PWD = 1,
    C_RETR = 2,
    C_PUT = 3,
    C_CWD = 4,
    C_DELE = 5,
    C_MKDIR = 6,
    C_RMDIR = 7,
    C_ERROR = 8,

    C_COUNT
} cmd_type_t;




cmd_type_t command_from_string(char * str);
char * command_to_string(cmd_type_t c);

#endif
