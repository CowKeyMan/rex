#define PORT 5000

#define NETWORK_BUFFER_SIZE 256

#define NETWORK_MAX_QUEUE 10

#define NETWORK_MASTER "Daniel"

#define STRING_BUFFER_SIZE 256
#define STRING_BUFFER_AMOUNT 16

char serverStartingCWD[128];

void writeMessage_ToSocket(char *message, int sockfd);

void writeMessage_ToHost(char *message, char *hostname);
void writeMessage_ToHost_GetResponse(char *message, char *hostname, char *responseBuffer);

// for client
int writeMessage_ToHost_ReturnSocket(char *message, char *hostname);

char *getHostName();

void continuouslyReadAndPrintFromSocketUntilEnd(int sockfd);

// for server
int startListening_ReturnSocket();
int acceptClient_ReturnNewSocket(int sockfd);

void readSocket_IntoBuffer(int sockfd, char *buffer);