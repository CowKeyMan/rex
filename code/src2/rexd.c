#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>

#include "Helper.h"
#include "Network.h"
#include "CommandsManager.h"
#include "StringManipulator.h"

void resetCWD();

char *paths[STRING_BUFFER_SIZE] = {
	"CURRENT WORKING DIRECTORY",
	"/usr/bin",
	"/bin",
	"/usr/local/bin"
};

void sig_handler(int sig){
  if (sig == SIGCHLD){
    int status;
    int i = waitpid(-1, &status, WNOHANG);
    if(i > 0 && kill(i, 0) != 0) {
        error("Error");
    }
  }
}

//Signal handler struct
struct sigaction sa;
void initializeSignalHandling();

// read client and process the inputs
void readClientCommand(int sockfd);

int main(int argc, char *argv[]){
  initializeSignalHandling();
  resetCWD();

  int sockfd = startListening_ReturnSocket();

  while(true){
    int newsockfd = acceptClient_ReturnNewSocket(sockfd);

    pid_t pid = fork();
    
    if (pid < 0) { // child
      close(sockfd);
      close(newsockfd);
      error("ERROR on fork");
    }
      
    if (pid == 0)  { // child
      close(sockfd);
      readClientCommand(newsockfd);
    }
    
    else{ // parent
      close(newsockfd);
    }
  }
}

void initializeSignalHandling(){
  // signal handler init
  sa.sa_handler = sig_handler;
  //Handle SIGINT, SIGCONT and SIGCHLD
  if(/*sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTSTP, &sa, NULL) == -1 ||*/ sigaction(SIGCHLD, &sa, NULL) == -1){
      error("ERROR, signals not catchable");
  }
}

void readClientCommand(int sockfd){
  
  char buffer[NETWORK_BUFFER_SIZE];
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
    serverAddCommand(args);
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