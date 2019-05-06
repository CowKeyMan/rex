/*****************************************************************************/
/*** exec-server.c                                                         ***/
/***                                                                       ***/
/*** Demonstrate how to execute external programs from a server.           ***/
/*****************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <signal.h>
#include <wait.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#define __USE_XOPEN
#include <resolv.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void PANIC(char* msg);
#define PANIC(msg)	{ perror(msg); exit(0); }

/*--------------------------------------------------------------------*/
/*--- Signal capture: SIGCHLD                                      ---*/
/*--------------------------------------------------------------------*/
void sig_handler(int sig)
{
	if ( sig == SIGCHLD )
	{	int retval;

		wait(&retval);
	}
}

/*--------------------------------------------------------------------*/
/*--- ConvertStrings - translate the CGI commands into strings     ---*/
/*--------------------------------------------------------------------*/
void ConvertStrings(char *string, char *strings[], int count)
{	int i=0, index=0;
	char *delimiters="?+";

	do
	{
		strings[index++] = string+i;
		while ( string[i] != 0  &&  strchr(delimiters, string[i]) == 0 )
			i++;
		if ( string[i] != 0 )
			string[i++] = 0;
	}
	while ( index < count-1  &&  string[i] != 0 );
	strings[index] = 0;
}

#define MAXPARAMS	20

/*----------------------------------------------------------------------*/
/*--- child - small HTTP remote command call (note lack of security!)---*/
/*----------------------------------------------------------------------*/
void child(void)
{	int i;
	char *s, *str, buffer[1024], *strings[MAXPARAMS];

	bzero(buffer, sizeof(buffer));
	if ( fgets(buffer, sizeof(buffer), stdin) != 0 )
	{
		if ( (s = strstr(buffer, " HTTP/")) != 0 )
			*s = 0;
		if ( strncasecmp(buffer, "GET ", 4) == 0 )
		{
			str = buffer+4;
			ConvertStrings(str, strings, MAXPARAMS);
			printf("HTTP/1.1 200 OK\nContent-Type: text/plain\n\n");
			execvp(strings[0], strings);
		}
	}
	printf("<html><body>ERROR IN REQUEST</body></html>\n\n");
}

/*--------------------------------------------------------------------*/
/*--- main - set up server, await connection, redirect I/O         ---*/
/*--------------------------------------------------------------------*/
int main()
{	int sd;
	struct sigaction act;
	struct sockaddr_in addr;

	bzero(&act, sizeof(act));
	act.sa_handler = sig_handler;
	act.sa_flags = SA_NOCLDSTOP | SA_RESTART;
	sigaction(SIGCHLD, &act, 0);

	sd = socket(PF_INET, SOCK_STREAM, 0);
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5001);
	addr.sin_addr.s_addr = INADDR_ANY;
	if ( bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
		PANIC("Bind");
	if ( listen(sd, 20) != 0 )
		PANIC("Listen");

	while (1)
	{	int client, size = sizeof(addr);

		client = accept(sd, (struct sockaddr*)&addr, &size);
		if ( client < 0 )
			perror("Accept");
		else
		{
			printf("Connected: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
			if ( !fork() )
			{
				close(sd);
				dup2(client, 0);
				dup2(client, 1);
				close(client);
				child();
				exit(0);
			}
			close(client);
		}
	}
	return 0;
}

