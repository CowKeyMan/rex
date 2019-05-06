#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "semaphore.h"

#include <stdio.h>

#include "Helper.h"
#include "Network.h"
#include "CommandsManager.h"
#include "StringManipulator.h"
#include "job.h"

void resetCWD();

char *paths[STRING_BUFFER_SIZE] = {
	"CURRENT WORKING DIRECTORY",
	"/usr/bin",
	"/bin",
	"/usr/local/bin"
};

// read client and process the inputs
void* readClientCommand(void *sockfd);

int main(int argc, char *argv[]){
  
  pthread_t thread;

  resetCWD();
  jobs_init();

  int sockfd = startListening_ReturnSocket();

  while(true){
    // continually accept clients
    int newsockfd = acceptClient_ReturnNewSocket(sockfd);
    
    //new thread to deal with client
		if(pthread_create(&thread, NULL, readClientCommand, (void*)&newsockfd)){
      // RETURN ERROR TO SOCKET
    }
  }
}

void* readClientCommand(void *_sockfd){
  
  int sockfd = *(int*)_sockfd;

  char buffer[STRING_BUFFER_SIZE];
  readSocket_IntoBuffer(sockfd, buffer);

  printf("%s\n", buffer);

  char *args[STRING_BUFFER_AMOUNT];
  splitStringBy(buffer, " ", args, STRING_BUFFER_AMOUNT);

  // analyze args
  if(strncmp("run", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
    serverRun(sockfd, paths, args);
  }else if(strncmp("submit", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
    //serverSubmit(newsockfd, paths, args, STRING_BUFFER_SIZE);
  }

  // only on master
  else if(strncmp("add", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
    serverAdd(sockfd, args);
  }
  else if(strncmp("change", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
  }
  else if(strncmp("getjid", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
  }
}

void resetCWD(){
	char buffer[STRING_BUFFER_SIZE];
	getcwd(buffer, STRING_BUFFER_SIZE);
	paths[0] = (char*)malloc(STRING_BUFFER_SIZE * sizeof(char));
	strncpy(paths[0], buffer, STRING_BUFFER_SIZE);
}