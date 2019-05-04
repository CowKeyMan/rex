#ifndef COMMANDS_MANAGER_H
#define COMMANDS_MANAGER_H

void forkChild(char **paths, char **args, int bufferSize);
bool changeCWD(char* newDir);

// run commands as seen by both client and server
void clientRun(int sockfd, char *message, char *buffer, int bufferSize);
void serverRun(int sockfd, char **paths, char **args, int bufferSize);

void clientSubmit(int sockfd, char *message, char *dateTime, char *buffer, int bufferSize);
void serverSubmit(int sockfd, char **paths, char **args, int bufferSize);

#endif // COMMANDS_MANAGER_H
