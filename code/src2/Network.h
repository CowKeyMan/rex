#define PORT 5000

#define NETWORK_BUFFER_SIZE 256

#define NETWORK_MAX_QUEUE 10

#define NETWORK_MASTER "Daniel"

#define STRING_BUFFER_SIZE 256
#define STRING_BUFFER_AMOUNT 16

void writeMessage_ToSocket(char *message, int sockfd);

void writeMessage_ToHost(char *message, char *hostname);
void writeMessage_ToHost_GetResponse(char *message, char *hostname, char *responseBuffer);