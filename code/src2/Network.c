#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#define __USE_XOPEN
#include <stdio.h>

#include "Network.h"
#include "Helper.h"

void writeMessage_ToSocket(char *message, int sockfd){
  // Send message to the server
  int n;
  if ((n = write(sockfd, message, strlen(message))) < 0){
    close(sockfd);
    error("ERROR writing to socket");
  }
}

void writeMessage_ToHost_GetResponse(char *message, char *hostname, char *responseBuffer){
  int sockfd, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char network_buffer[NETWORK_BUFFER_SIZE];

  bzero(network_buffer, strlen(network_buffer));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    error("ERROR opening socket");
  }

  // Get server name 
	if ((server = gethostbyname(hostname)) == NULL) {
		close(sockfd);
		error("ERROR, no such host\n");
	} 

	// Populate serv_addr structure 
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;  // Set to AF_INET
  bcopy((char *) server -> h_addr, // Set server address
        (char *) &serv_addr.sin_addr.s_addr,
                server -> h_length);
	serv_addr.sin_port = htons(PORT); // Set port (convert to network byte ordering)

	// Connect to the server 
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		error("ERROR connecting");
	}

  while (( n = write(sockfd, message, NETWORK_BUFFER_SIZE) ) > 0){
    bzero(network_buffer, NETWORK_BUFFER_SIZE);
  }
  if(n < 0){
    close(sockfd);
    error("ERROR writing to socket");
  }

  char buffer[NETWORK_BUFFER_SIZE];
  while( (n = read(sockfd, buffer, NETWORK_BUFFER_SIZE)) > 0){
    strncat(responseBuffer, buffer, sizeof(responseBuffer));
		bzero(network_buffer, NETWORK_BUFFER_SIZE);
  }
  if(n < 0){
    close(sockfd);
    error("ERROR writing to socket");
	}
  
	close(sockfd);
}

void writeMessage_ToHost(char *message, char *hostname){
  int sockfd, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char network_buffer[NETWORK_BUFFER_SIZE];

  bzero(network_buffer, strlen(network_buffer));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    error("ERROR opening socket");
  }

  // Get server name 
	if ((server = gethostbyname(hostname)) == NULL) {
		close(sockfd);
		error("ERROR, no such host\n");
	} 

	// Populate serv_addr structure 
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;  // Set to AF_INET
  bcopy((char *) server -> h_addr, // Set server address
        (char *) &serv_addr.sin_addr.s_addr,
                server -> h_length);
	serv_addr.sin_port = htons(PORT); // Set port (convert to network byte ordering)

	// Connect to the server 
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		error("ERROR connecting");
	}

  while (( n = write(sockfd, message, NETWORK_BUFFER_SIZE) ) > 0){
    bzero(network_buffer, NETWORK_BUFFER_SIZE);
  }
  if(n < 0){
    close(sockfd);
    error("ERROR writing to socket");
  }
  
	close(sockfd);
}

int writeMessage_ToHost_ReturnSocket(char *message, char *hostname){
  int sockfd, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  char network_buffer[NETWORK_BUFFER_SIZE];

  bzero(network_buffer, strlen(network_buffer));

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    error("ERROR opening socket");
  }

  // Get server name 
	if ((server = gethostbyname(hostname)) == NULL) {
		close(sockfd);
		error("ERROR, no such host\n");
	} 

	// Populate serv_addr structure 
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;  // Set to AF_INET
  bcopy((char *) server -> h_addr, // Set server address
        (char *) &serv_addr.sin_addr.s_addr,
                server -> h_length);
	serv_addr.sin_port = htons(PORT); // Set port (convert to network byte ordering)

	// Connect to the server 
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		error("ERROR connecting");
	}

  while (( n = write(sockfd, message, NETWORK_BUFFER_SIZE) ) > 0){
    bzero(network_buffer, NETWORK_BUFFER_SIZE);
  }
  if(n < 0){
    close(sockfd);
    error("ERROR writing to socket");
  }
  
	return sockfd;
}

char *getHostName(){
	char hostname[STRING_BUFFER_SIZE];
	hostname[STRING_BUFFER_SIZE - 1] = '\0';
	gethostname(hostname, sizeof(hostname));
	char *hn = hostname;

  return hn;
}

void continuouslyReadAndPrintFromSocketUntilEnd(int sockfd)
{
  int n;
  char buffer[NETWORK_BUFFER_SIZE];
  bzero(buffer, NETWORK_BUFFER_SIZE);
  while( (n = read(sockfd, buffer, NETWORK_BUFFER_SIZE)) > 0){
    printf("%s", buffer);
  }
  if(n < 0){
    close(sockfd);
    error("ERROR reading from socket");
	}
  close(sockfd);
}

int startListening_ReturnSocket(){
  int sockfd, pid, n;
  struct sockaddr_in serv_addr;

  // Create server socket (AF_INET, SOCK_STREAM)
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) , 0){
		error("ERROR opening socket");
	}

	// Initialize socket structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
	 
	// Bind the host address
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		error("ERROR on binding");
	}
  
  // Start listening for the clients
	listen(sockfd, NETWORK_MAX_QUEUE);
  
  return sockfd;
}

int acceptClient_ReturnNewSocket(int sockfd){
  int newsockfd, clilen;
  struct sockaddr_in cli_addr;

  // Accept connection form a client
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

  return newsockfd;
}

void readSocket_IntoBuffer(int sockfd, char *buffer){
  int n;
  bzero(buffer, NETWORK_BUFFER_SIZE);
  if ( (n = read(sockfd, buffer, NETWORK_BUFFER_SIZE)) < 0){
    close(sockfd);
    error("ERROR reading from socket");
  }
}