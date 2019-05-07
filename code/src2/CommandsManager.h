void executeCommand(char **paths, char **args);

void clientRun(char *message, char *destination);
void clientSubmit(char *message, char *destination);
void clientChdir(char *message, char *destination);
void clientStatus(char *destination);
void clientCopyToServer(char *fileName, char *destination, char *fileNameOnServer);
void clientCopyFromServer(char *fileName, char *destination, char *fileNameOnClient);
void clientKill(char *_jid, char *mode, char *_gracePeriod);

void serverRun(int sockfd, char ** paths, char **args);
void serverSubmit(int sockfd, char **args);
void serverChdir(int sockfd, char *dir);
void serverStatus(int sockfd);
void serverCopyFromClient(int sockfd, char *fileNameOnServer);
void serverCopyToClient(int sockfd, char *fileNameOnServer);
void serverKill(int sockfd, char **args);