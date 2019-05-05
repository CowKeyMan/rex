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

void serverRun(int sockfd, char ** paths, char **args){
  // redirection of stdio
  /*dup2(sockfd, STDIN_FILENO);
  dup2(sockfd, STDOUT_FILENO);
  dup2(sockfd, STDERR_FILENO);*/

  // create jobString to send to master
  char command[STRING_BUFFER_SIZE];
  concatenteStrings(args, command, STRING_BUFFER_SIZE);
  char hostName[STRING_BUFFER_SIZE];
  strncpy(hostName, getHostName(), STRING_BUFFER_SIZE);
  Job j = createJobNow(hostName, command, INTERACTIVE, RUNNING);
  char *jobString = jobToString(&j);

  //add the job to master's file
  char addJobCommand[STRING_BUFFER_SIZE];
  char JobIDString[STRING_BUFFER_SIZE];
  strncpy(addJobCommand, "add ", STRING_BUFFER_SIZE);
  strncat(addJobCommand, jobString, STRING_BUFFER_SIZE);
  writeMessage_ToHost_GetResponse(addJobCommand, NETWORK_MASTER, JobIDString);

  // fork child to execute the command
  pid_t pid = fork();
  if (pid == 0) {
    executeCommand(paths, args);
    error("Error");
  }else if (pid > 0) {
    // wait for child
    int status;
    waitpid(pid, &status, WUNTRACED);
  } else {
    fprintf(stderr, "Fork Failed");
  }

  // set the job to finished
  j.state = FINISHED;
  char changeJobCommand[STRING_BUFFER_SIZE];
  strncpy(changeJobCommand, "change ", STRING_BUFFER_SIZE);
  strncat(changeJobCommand, jobToString(&j), STRING_BUFFER_SIZE);
  writeMessage_ToHost(changeJobCommand, NETWORK_MASTER);
}

void serverAddCommand(char **args){
  char jobString[STRING_BUFFER_SIZE];
  concatenteStrings(args, jobString, STRING_BUFFER_SIZE);
  Job j = stringToJob(jobString);
  int jid = addJob(&j);
  /// send message to sender trough sockettt, the jid I guess? we'll see
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