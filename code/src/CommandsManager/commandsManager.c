#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/socket.h> 
#include <sys/types.h> 
#include <netinet/in.h>

#include "commandsManager.h"
#include "../NetworkItems/job.h"

void forkChild(char **paths, char **args, int bufferSize){
  pid_t pid = fork();

  if (pid > 0) {
  // wait for child
  int status;
  //waitpid(pid, &status, WUNTRACED);
  }else if (pid == 0) {
    //Ignore SIGINT and SIGTSTP
    //signal(SIGINT, SIG_IGN);
    //signal(SIGTSTP, SIG_IGN);

    //Go through each path one by one
    for (int i = 0; paths[i] != NULL; i++) {

      char pathtmp[bufferSize];
      strncpy(pathtmp, paths[i], bufferSize);
      strncat(pathtmp, "/", bufferSize);
      strncat(pathtmp, args[0], bufferSize);

      execv(pathtmp, args);
    }
    perror("Error");
    exit(-1);
  } else {
  	fprintf(stderr, "Fork Failed");
  }
}

bool changeCWD(char* newDir){
  //Change the directory
  if (chdir(newDir) != 0) {
    fprintf(stderr, "Invalid path\n");
    return false;
  } else {
    return true;
  }
}

void sendMessage(int sockfd, char *message){
  // Send message to the server
  int n;
  if ((n = write(sockfd, message, strlen(message))) < 0){
    perror("ERROR writing to socket");
    close(sockfd);
    exit(1);
  }
}

void clientRun(int sockfd, char *message, char *buffer, int bufferSize){
  strncpy(buffer, "run", bufferSize);
  strncat(buffer, " ", bufferSize);
  strncat(buffer, message, bufferSize);

  sendMessage(sockfd, buffer);
}

void serverRun(int sockfd, char ** paths, char **args, int bufferSize){
	dup2(sockfd, STDOUT_FILENO);

  forkChild(paths, args, bufferSize);
}

void clientSubmit(int sockfd, char *message, char *dateTime, char *buffer, int bufferSize){
  strncpy(buffer, "submit", bufferSize);
  strncat(buffer, " ", bufferSize);
  strncat(buffer, message, bufferSize);
  strncat(buffer, " ", bufferSize);
  strncat(buffer, dateTime, bufferSize);

  sendMessage(sockfd, buffer);
}

void serverSubmit(int sockfd, char **paths, char **args, int bufferSize){
	// see number of arguments (command + parameters + date + time)
	int numberOfArgs;
	for(numberOfArgs = 0; true; ++numberOfArgs){
		if(args[numberOfArgs] == NULL){
			break;
		}
	}

	// get hostname
	char hostname[STRING_BUFFER_SIZE];
	hostname[STRING_BUFFER_SIZE - 1] = '\0';
	gethostname(hostname, sizeof(hostname));
	char *a = hostname;

	// get command
	char command[STRING_BUFFER_SIZE];
	strncpy(command, args[0], STRING_BUFFER_SIZE);
	for(int i = 1; i < numberOfArgs - 2; ++i){
		strncat(command, " ", STRING_BUFFER_SIZE);
		strncat(command, args[i], STRING_BUFFER_SIZE);
	}

	// get date and time
	struct tm dateTime;
	int d,m,y,h,M,s;
	sscanf(args[numberOfArgs-2], "%d/%d/%d", &d, &m, &y);
	sscanf(args[numberOfArgs-1], "%d:%d:%d", &h, &M, &s);
	dateTime.tm_mday = d;
	dateTime.tm_mon = m;
	dateTime.tm_year = y;
	dateTime.tm_hour = h;
	dateTime.tm_min = M;
	dateTime.tm_sec = s;

	Job j;
	strncpy(j.command, command, STRING_BUFFER_SIZE);
	j.type = BATCH;
	j.state = WAITING;
	j.dateTime = dateTime;

	// get job id from master

	//addBatchJob();
	
	//send add job to master
}

void getJidFromMaster(Job *j){
	//socket stuff
	int sockfd, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char network_buffer[NETWORK_BUFFER_SIZE];
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
			perror("ERROR opening socket");
			exit(1);
	}

	bzero(network_buffer, strlen(network_buffer));
}
