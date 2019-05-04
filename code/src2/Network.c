#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#define __USE_XOPEN

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
  while( (n = read(sockfd, buffer, NETWORK_BUFFER_SIZE)) < 0){
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