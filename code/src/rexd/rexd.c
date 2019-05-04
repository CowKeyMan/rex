#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>

#include "../NetworkItems/networkOptions.h"
#include "../StringManipulator/StringManipulator.h"
#include "../CommandsManager/commandsManager.h"

void signalHandler(int signalNumber);

//process the given line
void computeLine(char* line);
//set current working directory
void resetCWD();

//Signal handler struct
//struct sigaction sa;

//Used to store the default STDIN, OUT, ERR for reverting them after a command
int stdinFileDescriptor;
int stdoutFileDescriptor;
int stderrFileDescriptor;

char *paths[STRING_BUFFER_SIZE] = {
	"CURRENT WORKING DIRECTORY",
	"/usr/bin",
	"/bin",
	"/usr/local/bin"
};

// socket stuff
int sockfd, newsockfd, clilen, pid, n;
struct sockaddr_in serv_addr, cli_addr;

void startSocket();
void listenAndAccept();
void childProcess(int sock);

bool isMaster = false;

int main(int argc, char *argv[]){
	resetCWD();

	//Duplicate the default file descriptors
	stdinFileDescriptor = dup(STDIN_FILENO);
	stdoutFileDescriptor = dup(STDOUT_FILENO);
	stderrFileDescriptor = dup(STDERR_FILENO);

	startSocket();
	listenAndAccept();
}

void startSocket(){
	// Create server socket (AF_INET, SOCK_STREAM)
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) , 0){
		perror("ERROR opening socket");
		exit(EXIT_FAILURE);
	}

	// Initialize socket structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(SERVER_PORT);
	 
	// Bind the host address
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		close(sockfd);
		perror("ERROR on binding");
		exit(EXIT_FAILURE);
	}
}

void listenAndAccept(){
	// Start listening for the clients
	listen(sockfd, NETWORK_MAX_QUEUE);
	 
	 // Infinite loop
	clilen = sizeof(cli_addr);
	while (1) {
		 
		// Accept connection form a client
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		
		if (newsockfd < 0)  {
			close(sockfd);
			perror("ERROR on accept");
			exit(EXIT_FAILURE);
		}
			
		// Accept successful, fork a new process
		pid = fork();

		if (pid < 0) {
			close(sockfd);
			close(newsockfd);
			perror("ERROR on fork");
			exit(EXIT_FAILURE);
		}
			
		// If this is the child, serve client
		if (pid == 0)  {
			close(sockfd);
			childProcess(newsockfd);
			exit(0);
		}
		else{
			// Otherwise this is the parent, close the new socket
			// and wait for another client
			close(newsockfd);
		}
		// wait(&pid);
		
		// NOTE: you should call wait for created processes to 
		// clear out child data in OS
	
	}
}

void childProcess(int sock){
	char buffer[NETWORK_BUFFER_SIZE];
      
	bzero(buffer,NETWORK_BUFFER_SIZE);
   
	if ((n = read(sock,buffer,NETWORK_BUFFER_SIZE)) < 0) {
		close(sock);
		perror("ERROR reading from socket");
		exit(EXIT_FAILURE);
	}
   
	computeLine(buffer);
}

void computeLine(char* line){
	char *args[STRING_BUFFER_AMOUNT];
	//printf("%s\n", line);
	splitStringBy(line, " ", args, STRING_BUFFER_AMOUNT);

	if(strncmp("run", args[0], STRING_BUFFER_SIZE) == 0){
		shiftStrings(args);
		serverRun(newsockfd, paths, args, STRING_BUFFER_SIZE);
	}else if(strncmp("submit", args[0], STRING_BUFFER_SIZE) == 0){
		shiftStrings(args);
		serverSubmit(newsockfd, paths, args, STRING_BUFFER_SIZE);
	}else if(strncmp("add", args[0], STRING_BUFFER_SIZE) == 0){
		shiftStrings(args);
		//add job
	}
	// only on master
	else if(strncmp("getjid", args[0], STRING_BUFFER_SIZE) == 0){
		
	}

}

void resetCWD(){
	char buffer[STRING_BUFFER_SIZE];
	getcwd(buffer, STRING_BUFFER_SIZE);
	paths[0] = (char*)malloc(STRING_BUFFER_SIZE * sizeof(char));
	strncpy(paths[0], buffer, STRING_BUFFER_SIZE);
	printf("CWD: %s\n", paths[0]);
}
