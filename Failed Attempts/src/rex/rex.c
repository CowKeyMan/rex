#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>
#define __USE_XOPEN
#include <time.h>

#include "../StringManipulator/StringManipulator.h"
#include "../CommandsManager/commandsManager.h"
#include "../NetworkItems/networkOptions.h"

#define NETWORK_BUFFER_SIZE 256

// process and do stuff with the arguments given
void computeArgs(char **args);
bool extractDestination(char *arg);
void extractDestinationHard(char *arg); // exit if could not extract destination
void *extractTime(char *date, char *time, char *ret); //return string containing date and time

void acceptResponse();

//socket stuff
int sockfd, n;
struct sockaddr_in serv_addr;
struct hostent *server;
char network_buffer[NETWORK_BUFFER_SIZE];
void startSocket();
void setServer();

char destination[STRING_BUFFER_SIZE];
char parameter[STRING_BUFFER_SIZE];

int main(int argc, char *argv[]){
	startSocket();
	computeArgs(argv);

	acceptResponse();
}

void computeArgs(char** args){
	//splitStringBy(line, " ", args, STRING_BUFFER_AMOUNT);

	// if no command is found
	if(! args[1]){
		fprintf(stderr, "No command found\n");
		return;
	}else{
		if(! args[2]){
			fprintf(stderr, "No parameter found\n");
			return;
		}
	}
	if(strncmp("run", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
		clientRun(sockfd, parameter, network_buffer, NETWORK_BUFFER_SIZE);
	}else if(strncmp("submit", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
		if(true){//(char *dateTime = extractTime(args[3], args[4]))){
			char dateTime[STRING_BUFFER_SIZE];
			extractTime(args[3], args[4], dateTime);
			clientSubmit(sockfd, parameter, dateTime, network_buffer, NETWORK_BUFFER_SIZE);
		}else{
			fprintf(stderr, "Error, date and time specified incorrectly");
		}
	}else if(strncmp("kill", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
	}else if(strncmp("status", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
	}else if(strncmp("copy", args[1], STRING_BUFFER_SIZE) == 0){
		if(extractDestination(args[2])){

		}
		else if(args[3] && extractDestination(args[3])){

		}
	}else if(strncmp("chdir", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
	}else{
		fprintf(stderr, "Command not found\n");
	}
}

bool extractDestination(char *arg){
	char *args[STRING_BUFFER_AMOUNT];
	splitStringBy(arg, ":", args, STRING_BUFFER_SIZE);

	if(args[1] != NULL){
		strncpy(destination, args[0], STRING_BUFFER_SIZE);
		strncpy(parameter, args[1], STRING_BUFFER_SIZE);

		setServer();
		return true;
	}

	return false;
}

void extractDestinationHard(char *arg){
	if(!extractDestination(arg)){
			fprintf(stderr, "ERROR incorrect use. Please enter address after command\n");
			exit(EXIT_FAILURE);
	}
}

void startSocket(){
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
			perror("ERROR opening socket");
			exit(1);
	}

	bzero(network_buffer, strlen(network_buffer));
}

void setServer(){
	// Get server name 
	if ((server = gethostbyname(destination)) == NULL) {
		fprintf(stderr,"ERROR, no such host\n");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

	// Populate serv_addr structure 
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;  // Set to AF_INET
	bcopy((char *) server -> h_addr, // Set server address
				(char *) &serv_addr.sin_addr.s_addr,
									server -> h_length);
	serv_addr.sin_port = htons(SERVER_PORT); // Set port (convert to network byte ordering)

	// Connect to the server 
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR connecting");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
}

void *extractTime(char *date, char *_time, char *ret){
	time_t retTime;
	struct tm *t;
	if(date == NULL || strncmp("now", date, STRING_BUFFER_SIZE) == 0){
		retTime = time(NULL);
		t = localtime(&retTime);
		strftime(ret, STRING_BUFFER_SIZE, "%d/%m/%Y %H:%M:%S", t);
	}else if (date && _time){ // both are not null
		int d,m,y,h,M,s;
		if( sscanf(date, "%d/%d/%d", &d, &m, &y) == 3
		&& sscanf(_time, "%d:%d:%d", &h, &M, &s) == 3){
			sprintf(ret, "%d/%d/%d %d:%d:%d", d, m, y, h, M, s);
		 }else{
			fprintf(stderr, "Invalid date and time\n");
			exit(EXIT_FAILURE);
		 }
	}else{
		fprintf(stderr, "Invalid date and time\n");
		exit(EXIT_FAILURE);
	}
}

void acceptResponse(){
	while(1){
		bzero(network_buffer, NETWORK_BUFFER_SIZE);
	 	if ((n = read(sockfd, network_buffer, NETWORK_BUFFER_SIZE)) < 0){
     	perror("ERROR reading from socket");
		 	close(sockfd);
     	exit(EXIT_FAILURE);
   	}else if(n == 0){
			break;
		}
    printf("%s", network_buffer);
	}
	close(sockfd);
}
