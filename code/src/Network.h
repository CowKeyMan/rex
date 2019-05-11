#define PORT 5000 // port over which server listens and communicates on

#define NETWORK_BUFFER_SIZE 256

#define NETWORK_MAX_QUEUE 10

#define NETWORK_MASTER "Daniel"

#define STRING_BUFFER_SIZE 256
#define STRING_BUFFER_AMOUNT 16

char serverStartingCWD[128];

void writeMessage_ToSocket(char *message, int sockfd); // send a message over a socket

void writeMessage_ToHost(char *message, char *hostname); // create socket, conect to hostname, send message over it, close socket
void writeMessage_ToHost_GetResponse(char *message, char *hostname, char *responseBuffer); // same as above, but wait for a response

// for client
int writeMessage_ToHost_ReturnSocket(char *message, char *hostname); // write a message to the specified host and return the messsage by which the client was accepted, since this uses the connect method

char *getHostName(); // get the name of the current host

void continuouslyReadAndPrintFromSocketUntilEnd(int sockfd); // close socket at end

// for server
int startListening_ReturnSocket(); // start listening on set port and return the listening socket
int acceptClient_ReturnNewSocket(int sockfd); // accept a client on the required socket, and return a new socket used for the client

void readSocket_IntoBuffer(int sockfd, char *buffer); // Read text from a socket and modify the buffer contents to the messsage
