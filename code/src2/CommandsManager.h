void executeCommand(char **paths, char **args);

void clientRun(char *message, char *destination);
void clientSubmit(char *message, char *destination);

void serverRun(int sockfd, char ** paths, char **args);
void serverSubmit(int sockfd, char **args);