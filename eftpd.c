#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "server.h"
#include "user.h"
#include "protocol.h"
#include "misc.h"
#include "bool.h"
#include "assert.h"

#define DEFAULT_USER_FILE "/etc/eftpd/users"

void usage(int ev)
{
    fputs("usage: eftpd [options]\n"
          "  Start the eftpd server.\n\n"
	  "  Accepted options:\n"
          "   -h                 print this help and quit\n"
	  "   -d                 execute server as a system daemon\n"
	  "   -r <dir>           initial current directory\n"
	  "   -u <file>          specify the user configuration file\n"
	  "                      (default is " DEFAULT_USER_FILE  ")\n"
	  "   -p <port>          use 'port' instead of the default port (" MXSTR(SERVER_DEFAULT_PORT) ")\n"
	  "\n"
	  , stderr);
    exit(ev);
}

void stop_server_handler(int s)
{
    stop_server();
}

void child_stopped(int s)
{
    wait(NULL);
}

int main(int argc, char ** argv)
{
    int optch;

    bool exe_daemon = false;
    port_t port = SERVER_DEFAULT_PORT;
    char * ch_dir = NULL;

    FILE * fusers = NULL;

    while( (optch = getopt(argc, argv, "hdp:u:r:")) != EOF )
    {
	switch(optch)
	{
	case 'u':
	    if(fusers) fclose(fusers);
	    fusers = fopen(optarg, "r");
	    if(!fusers)
	    {
		fprintf(stderr, "eftpd: unable to open `%s' for reading\n", optarg);
		return EXIT_FAILURE;
	    }
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	case 'd':
	    exe_daemon = true;
	    break;
	case 'r':
	    ch_dir = optarg;
	    break;
	case 'h':
	    usage(EXIT_SUCCESS);
	    break;
	default:
	    usage(EXIT_FAILURE);
	    break;
	}
    }

    if(!fusers)
    {
	fusers = fopen(DEFAULT_USER_FILE, "r");
	if(!fusers)
	{
	    fputs("eftpd: unable to open default user file`" DEFAULT_USER_FILE "'\n",
		stderr);
	    return EXIT_FAILURE;
	}
    }

    user_pool_t * pool = create_user_pool();

    read_users_from_file(pool, fusers);
    fclose(fusers);

    struct sigaction nv, old;
    memset(&nv, 0, sizeof(nv));
    nv.sa_handler = &stop_server_handler;

    sigaction(SIGTERM, &nv, &old);
    sigaction(SIGINT, &nv, &old);

    nv.sa_handler = &child_stopped;

    sigaction(SIGCHLD, &nv, &old);


    if(exe_daemon)
    {
	dbg_printf("daemonisation...\n");
	if(daemon(0, 1) != 0)
	{
	    perror("daemon");
	    return EXIT_FAILURE;
	}
    }

    if(ch_dir)
	chdir(ch_dir);

    int r = start_server(pool, port);

    free_user_pool(pool);

    return r;
}
