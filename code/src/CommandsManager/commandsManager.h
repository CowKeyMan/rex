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

void forkChild(char **paths, char **args, int bufferSize, bool wait);
bool changeCWD(char* newDir);

void clientRun(int sockfd, char *message, char *buffer, int bufferSize);
