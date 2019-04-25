#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>

void forkChild(char **paths, char **args, int bufferSize, bool wait);
bool changeCWD(char* newDir);
