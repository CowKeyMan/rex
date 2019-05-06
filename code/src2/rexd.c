#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "semaphore.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>

#include "Helper.h"
#include "Network.h"
#include "CommandsManager.h"
#include "StringManipulator.h"
#include "job.h"

void *pollThread();
void resetCWD();

char *paths[STRING_BUFFER_SIZE] = {
	"CURRENT WORKING DIRECTORY",
	"/usr/bin",
	"/bin",
	"/usr/local/bin"
};
pthread_t pollingThread;

// read client and process the inputs
void* readClientCommand(void *sockfd);

int main(int argc, char *argv[]){
  pthread_t thread;

  resetCWD();
  jobs_init();
	remove(JOBS_FILENAME); // reset jobs file, mostly for easier debugging

  //new thread to deal with client
  if(pthread_create(&pollingThread, NULL, pollThread, NULL)){
    error("Error creting process for you. Bye!");
  }

  int sockfd = startListening_ReturnSocket();

  while(true){
    // continually accept clients
    int newsockfd = acceptClient_ReturnNewSocket(sockfd);
    
    //new thread to deal with client
		if(pthread_create(&thread, NULL, readClientCommand, (void*)&newsockfd)){
      writeMessage_ToSocket("Error creting process for you. Bye!", newsockfd);
      close(newsockfd);
    }
  }
}

void* readClientCommand(void *_sockfd){
  
  int sockfd = *(int*)_sockfd;

  char buffer[STRING_BUFFER_SIZE];
  readSocket_IntoBuffer(sockfd, buffer);

  printf("From rexd readclientCommand: %s\n", buffer);

  char *args[STRING_BUFFER_AMOUNT];
  splitStringBy(buffer, " ", args, STRING_BUFFER_AMOUNT);

  // analyze args
  if(strncmp("run", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
    serverRun(sockfd, paths, args);
  }else if(strncmp("submit", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
    serverSubmit(sockfd, args);
    close(sockfd);
  }

  // only on master
  else if(strncmp("add", args[0], STRING_BUFFER_SIZE) == 0){
    shiftStrings(args);
    //serverAdd(sockfd, args);
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

void *pollThread(){
  while(1){
    if(&jobs[0] != NULL){
      time_t t = time(NULL);
      struct tm *lt = localtime(&t);
      lt->tm_year += 1900;
      Job topJob = jobs[ sizeof(jobs)/sizeof(Job) - 1 ];
      if( timeBiggerThan( lt , &topJob.dateTime) ) {
      printf("helo\n");
        //GET JOB, OPEN FILE, redirect stdout of child to file & stderr, close stdin of child, in parent change the child, in signal handler for child lookup child by pid and change the status to finished
        topJob.state = RUNNING;
        changeJob(&topJob);

        // fork child to execute the command
        pid_t pid = fork();
        if (pid == 0) {
          int f;
          char fileName[STRING_BUFFER_SIZE];
          sprintf(fileName, "Job_%d.txt", topJob.jid);
          if( !(f = open(fileName, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR) ) ) {
            error("Error creating log file.");
          }

          // redirection of stdio
          close(STDIN_FILENO);
          dup2(f, STDOUT_FILENO);
          dup2(f, STDERR_FILENO);

          char *args[STRING_BUFFER_AMOUNT];
          splitStringBy(topJob.command, " ", args, STRING_BUFFER_AMOUNT);
          executeCommand(paths, args);
          exit(1);
        }else if (pid > 0) {
          removeTopJob();
        } else {
          FILE *f;
          char fileName[STRING_BUFFER_SIZE];
          sprintf(fileName, "Job_%d.txt", topJob.jid);
          if( !(f=fopen(fileName, "w")) ) {
            error("Error opening file.");
          }
          fputs("Fork Failed", f);
          fclose(f);
        }
      }
    }
    sleep(1);
  }
}