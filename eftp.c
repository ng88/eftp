#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "assert.h"
#include "bool.h"
#include "client.h"
#include "protocol.h"
#include "misc.h"
#include "vector.h"



void usage(int ev)
{
    fputs("usage: eftp [options] host\n"
          "  Connect to a eftpd server on specified host.\n\n"
	  "  Accepted options:\n"
          "   -h                 print this help and quit\n"
	  "   -p <port>          use 'port' instead of the default port (" MXSTR(SERVER_DEFAULT_PORT) ")\n"
	  "\n"
	  , stderr);
    exit(ev);
}



void stop_client_handler(int s)
{
    static int st = 0;
    st = 1 - st;

    if(st == 1)
	stop_client();
}



int main(int argc, char ** argv)
{

    int optch;

    port_t port = SERVER_DEFAULT_PORT;

    while( (optch = getopt(argc, argv, "hp:")) != EOF )
    {
	switch(optch)
	{
	case 'p':
	    port = atoi(optarg);
	    break;
	case 'h':
	    usage(EXIT_SUCCESS);
	    break;
	default:
	    usage(EXIT_FAILURE);
	    break;
	}
    }

    /*       HOST       */
    if(argc - optind < 1)
    {
	fputs("eftp: argument missing.\n", stderr);
	usage(EXIT_FAILURE);
    }

    struct sigaction nv, old;
    memset(&nv, 0, sizeof(nv));
    nv.sa_handler = &stop_client_handler;

    sigaction(SIGTERM, &nv, &old);
    sigaction(SIGINT, &nv, &old);
    sigaction(SIGCHLD, &nv, &old);

    int ret = connect_to_server(argv[optind], port);

    return ret;
}
