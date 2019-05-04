#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "Network.h"
#include "Helper.h"
#include "CommandsManager.h"

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

  writeMessage_ToHost_GetResponse(buffer, destination, buffer);

  printf("%s", buffer);
}

void serverRun(int sockfd, char ** paths, char **args){
	dup2(sockfd, STDOUT_FILENO);

  char command[STRING_BUFFER_SIZE];
  concatenteStrings(args, command, STRING_BUFFER_SIZE);

  createJobNow(getHostName(), command, INTERACTIVE, RUNNING);

  //add the job to master

  executeCommand(paths, args);

  // set the job to finished

}