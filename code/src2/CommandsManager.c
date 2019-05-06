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
    
    int jid = addJob(&j);
    char jobString[STRING_BUFFER_SIZE];
  	jobToString(&j, jobString);
    strncpy(hostName, jobString, STRING_BUFFER_SIZE);
    printf("%s\n", hostName);
    // wait for child until it stops ab/normally
    int status = -1;
    while( !(WIFSIGNALED(status) || WIFEXITED(status)) ){
      waitpid(pid, &status, WUNTRACED);
    }

    close(sockfd);
    // chnge job to finished with its jid
    if(WIFSIGNALED(status)){
      j.state = TERMINATED;
    }else if(WIFSIGNALED(status)){
      j.state = FINISHED;
    }
    changeJob(&j);
  } else {
    fprintf(stderr, "Fork Failed");
  }
  // set the job to finished
  /*j.state = FINISHED;
  char changeJobCommand[STRING_BUFFER_SIZE];
  strncpy(changeJobCommand, "change ", STRING_BUFFER_SIZE);
  strncat(changeJobCommand, jobToString(&j), STRING_BUFFER_SIZE);
  writeMessage_ToHost(changeJobCommand, NETWORK_MASTER);*/
}

void serverAdd(int sockfd, char **args){
  char jobString[STRING_BUFFER_SIZE];
  concatenteStrings(args, jobString, STRING_BUFFER_SIZE);
  Job j = stringToJob(jobString);
  int jid = addJob(&j);
  char jidString[8];
  sprintf(jidString, "%d\n", jid);
  writeMessage_ToSocket(jidString, sockfd);
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