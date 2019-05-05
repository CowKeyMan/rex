void executeCommand(char **paths, char **args);

void clientRun(char *message, char *destination);

void serverRun(int sockfd, char ** paths, char **args);

void serverAdd(int sockfd, char **args);