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
#include <stdbool.h>

#include <stdio.h>

#include "Helper.h"
#include "Network.h"
#include "CommandsManager.h"
#include "StringManipulator.h"
#include "job.h"

void *pollThread();
void *handleChild(void *job);
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
void resetOutputFileDirectory();

int main(int argc, char *argv[]){
  resetOutputFileDirectory();

  pthread_t thread;

  resetCWD();
  jobs_init();

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

  printf("Command from client: %s\n", buffer);

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
  }else if(strncmp("status", args[0], STRING_BUFFER_SIZE) == 0){
    serverStatus(sockfd);
    close(sockfd);
  }else if(strncmp("chdir", args[0], STRING_BUFFER_SIZE) == 0){
    serverChdir(sockfd, args[1]);
    close(sockfd);
  }else if(strncmp("copyFromServer", args[0], STRING_BUFFER_SIZE) == 0){
    serverCopyToClient(sockfd, args[1]);
    close(sockfd);
  }else if(strncmp("copyToServer", args[0], STRING_BUFFER_SIZE) == 0){
    serverCopyFromClient(sockfd, args[1]);
    close(sockfd);
  }
}

void resetCWD(){
	char buffer[STRING_BUFFER_SIZE];
	getcwd(buffer, STRING_BUFFER_SIZE);
	paths[0] = (char*)malloc(STRING_BUFFER_SIZE * sizeof(char));
	strncpy(paths[0], buffer, STRING_BUFFER_SIZE);
	strncpy(serverStartingCWD, buffer, STRING_BUFFER_SIZE);
}

void *pollThread(){
  while(1){
    int noOfBatchJobs = getNoOfBatchJobs();
    if(noOfBatchJobs > 0){
      time_t t = time(NULL);
      struct tm *lt = localtime(&t);
      lt->tm_mon += 1;
      lt->tm_year += 1900;
      Job topJob = jobs[noOfBatchJobs - 1];

      
      if( timeBiggerThan( lt , &topJob.dateTime) == true ) {
        topJob.state = RUNNING;
        changeJob(&topJob);
        //GET JOB, OPEN FILE, redirect stdout of child to file & stderr, close stdin of child, in parent change the child, in signal handler for child lookup child by pid and change the status to finished

        // fork child to execute the command
        pid_t pid = fork();
        if (pid == 0) {
          int f;
          char fileName[STRING_BUFFER_SIZE];
          sprintf(fileName, "%s/Jobs/Job_%d.txt", serverStartingCWD, topJob.jid);
          remove(fileName);
          if( !(f = open(fileName, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR) ) ) {
            error("Error creating log file.");
          }

          char *args[STRING_BUFFER_AMOUNT];
          splitStringBy(topJob.command, " ", args, STRING_BUFFER_AMOUNT);
          
          // redirection of stdio
          close(STDIN_FILENO);
          dup2(f, STDOUT_FILENO);
          dup2(f, STDERR_FILENO);

          executeCommand(paths, args);
        }else if (pid > 0) {
          topJob.pid = pid;
          Job *job = (Job*)malloc(sizeof(Job));
          *job = topJob;
          pthread_t childHandlerThread;
          if(pthread_create(&childHandlerThread, NULL, handleChild, job)){
            perror("Error in creting child handling socket");
          }
        } else {
          FILE *f;
          char fileName[STRING_BUFFER_SIZE];
          sprintf(fileName, "%s/Jobs/Job_%d.txt", serverStartingCWD, topJob.jid);
          if( !(f=fopen(fileName, "w")) ) {
            error("Error opening file.");
          }
          fputs("Fork Failed", f);
          fclose(f);
        }
        removeTopJob();
      }
    }
    sleep(1); // poll every 1 second, rather than constantly
  }
  pthread_exit(NULL);
}

void *handleChild(void *job){
  Job j = *(Job*)job;
  free(job);
  // wait for child until it stops ab/normally
  int status = -1;
  while( !(WIFSIGNALED(status) || WIFEXITED(status)) ){
    waitpid(j.pid, &status, WUNTRACED);
  }

  // chnge job to finished with its jid
  if(WIFSIGNALED(status)){
    j.state = TERMINATED;
  }else if(WIFEXITED(status)){
    j.state = FINISHED;
  }
  changeJob(&j);
  pthread_exit(NULL);
}

void resetOutputFileDirectory(){
  mkdir("Jobs", S_IRUSR | S_IWUSR | S_IXUSR);
  remove(JOBS_FILENAME);
  for(int i = 0; remove("Jobs") == -1; ++i) {
    FILE *f;
    char fileName[STRING_BUFFER_SIZE];
    sprintf(fileName, "Jobs/Job_%d.txt", i);
    if((f=fopen(fileName, "r")) ) {
      fclose(f);
      remove(fileName);
    }
  }
  mkdir("Jobs", S_IRUSR | S_IWUSR | S_IXUSR);
  FILE *f = fopen(JOBS_FILENAME, "w");
  fclose(f);
}