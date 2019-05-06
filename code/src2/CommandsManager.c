#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>

#include "Network.h"
#include "Helper.h"
#include "CommandsManager.h"
#include "job.h"
#include "StringManipulator.h"

void executeCommand(char **paths, char **args){
  for (int i = 0; paths[i] != NULL; i++) {
    char pathtmp[STRING_BUFFER_SIZE];
    strncpy(pathtmp, paths[i], STRING_BUFFER_SIZE);
    strncat(pathtmp, "/", STRING_BUFFER_SIZE);
    strncat(pathtmp, args[0], STRING_BUFFER_SIZE);

    execv(pathtmp, args);
  }
  error("Error");
}

void clientRun(char *message, char *destination){
  char buffer[STRING_BUFFER_SIZE];

  strncpy(buffer, "run", STRING_BUFFER_SIZE);
  strncat(buffer, " ", STRING_BUFFER_SIZE);
  strncat(buffer, message, STRING_BUFFER_SIZE);

  // send server run _ _ _
  int sockfd = writeMessage_ToHost_ReturnSocket(buffer, destination);
  continuouslyReadAndPrintFromSocketUntilEnd(sockfd);
}

void clientSubmit(char *message, char *destination){
  char buffer[STRING_BUFFER_SIZE];

  strncpy(buffer, "submit", STRING_BUFFER_SIZE);
  strncat(buffer, " ", STRING_BUFFER_SIZE);
  strncat(buffer, message, STRING_BUFFER_SIZE);

  // send server submit _ _ _
  char response[STRING_BUFFER_SIZE];
  writeMessage_ToHost_GetResponse(buffer, destination, response);
  printf("%s\n", response);
}

#include <stdlib.h>
void serverRun(int sockfd, char ** paths, char **args){
  // fork child to execute the command
  pid_t pid = fork();
  if (pid == 0) {
    // redirection of stdio
    dup2(sockfd, STDIN_FILENO);
    dup2(sockfd, STDOUT_FILENO);
    dup2(sockfd, STDERR_FILENO);
    executeCommand(paths, args);
  }else if (pid > 0) {

    //create jobString to send to file
    char command[STRING_BUFFER_SIZE];
    concatenteStrings(args, command, STRING_BUFFER_SIZE);
    char hostName[STRING_BUFFER_SIZE];
    strncpy(hostName, getHostName(), STRING_BUFFER_SIZE);
    Job j = createJobNowPid(pid, hostName, command, INTERACTIVE, RUNNING);
    
    addJob(&j);
    char jobString[STRING_BUFFER_SIZE];
  	jobToString(&j, jobString);

    // wait for child until it stops ab/normally
    int status = -1;
    while( !(WIFSIGNALED(status) || WIFEXITED(status)) ){
      waitpid(pid, &status, WUNTRACED);
    }

    close(sockfd);
    // chnge job to finished with its jid
    if(WIFSIGNALED(status)){
      j.state = TERMINATED;
    }else if(WIFEXITED(status)){
      j.state = FINISHED;
    }
    changeJob(&j);
  } else {
    writeMessage_ToSocket("Fork Failed", sockfd);
  }
}

void serverSubmit(int sockfd, char **args){
  // redirection of stderr in case of errors in the middle of submitting
  dup2(sockfd, STDERR_FILENO);

  //create jobString to send to file
  char *date = args[0];
  char *_time = args[1];
  shiftStrings(args);
  shiftStrings(args);
  struct tm t;
  sscanf(date, "%d/%d/%d", &t.tm_mday, &t.tm_mon, &t.tm_year);
  sscanf(_time, "%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec);
  char command[STRING_BUFFER_SIZE];
  concatenteStrings(args, command, STRING_BUFFER_SIZE);
  char hostName[STRING_BUFFER_SIZE];
  strncpy(hostName, getHostName(), STRING_BUFFER_SIZE);
  Job j = createJob(hostName, command, BATCH, WAITING, &t);

  addJob(&j);
  
  char jobString[STRING_BUFFER_SIZE];
  jobToString(&j, jobString);

  addBatchJob(&j);
  writeMessage_ToSocket("Acknowledged", sockfd);
  close(sockfd);
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