#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>

void forkChild(char **paths, char **args, int bufferSize);
bool changeCWD(char* newDir);

// run commands as seen by both client and server
void clientRun(int sockfd, char *message, char *buffer, int bufferSize);
void serverRun(int sockfd, char ** paths, char **args, int bufferSize);

void clientSubmit(int sockfd, char *message, char *dateTime, char *buffer, int bufferSize);
