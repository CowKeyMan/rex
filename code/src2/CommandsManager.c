#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

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

void clientChdir(char *message, char *destination){
  char buffer[STRING_BUFFER_SIZE];

  strncpy(buffer, "chdir", STRING_BUFFER_SIZE);
  strncat(buffer, " ", STRING_BUFFER_SIZE);
  strncat(buffer, message, STRING_BUFFER_SIZE);

  // send server chdir
  char response[STRING_BUFFER_SIZE];
  writeMessage_ToHost_GetResponse(buffer, destination, response);
  printf("%s\n", response);
}

void clientStatus(char *destination){
  // send server status
  int sockfd = writeMessage_ToHost_ReturnSocket("status", destination);
  continuouslyReadAndPrintFromSocketUntilEnd(sockfd);
}

void clientCopyFromServer(char *fileName, char *destination, char *fileNameOnClient){
  char message[STRING_BUFFER_SIZE];
  strncpy(message, "copyFromServer ", STRING_BUFFER_SIZE);
  strncat(message, fileName, STRING_BUFFER_SIZE);

  remove(fileNameOnClient);

  int f;
  if( !(f = open(fileNameOnClient, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR) ) ) {
    error("Error creating file");
  }

  int sockfd = writeMessage_ToHost_ReturnSocket(message, destination);

  // fork child to execute the command
  pid_t pid = fork();
  if (pid == 0) {
    // redirection of stdio
    dup2(f, STDOUT_FILENO);
    continuouslyReadAndPrintFromSocketUntilEnd(sockfd);
    close(f);
    exit(EXIT_SUCCESS);
  }else if (pid > 0) {
    // wait for child until it stops ab/normally
    int status = -1;
    while( !(WIFSIGNALED(status) || WIFEXITED(status)) ){
      waitpid(pid, &status, WUNTRACED);
    }
    close(sockfd);
    close(f);
  } else {
    error("Fork Failed");
  }
}
void clientCopyToServer(char *fileName, char *destination, char *fileNameOnServer){
  char message[STRING_BUFFER_SIZE];
  strncpy(message, "copyToServer ", STRING_BUFFER_SIZE);
  strncat(message, fileNameOnServer, STRING_BUFFER_SIZE);

  FILE *f;

	if( !(f=fopen(fileName, "r")) ) {
    error("Error opening file.");
	}

  int sockfd = writeMessage_ToHost_ReturnSocket(message, destination);

  char line [STRING_BUFFER_SIZE];
	while ( fgets ( line, STRING_BUFFER_SIZE, f ) ){
    writeMessage_ToSocket(line, sockfd);
  }

  close(sockfd);
  fclose(f);
}

void clientKill(char *_jid, char *mode, char *_gracePeriod){
  char message[STRING_BUFFER_SIZE];
  int jid = 0;
  if( (jid = atoi(_jid)) <= 0 ){
    error("Invalid job ID");
  }
  int gracePeriod = atoi(_gracePeriod);
  if(!(strncmp(mode, "soft", STRING_BUFFER_SIZE) == 0
    || strncmp(mode, "hard", STRING_BUFFER_SIZE) == 0
    || strncmp(mode, "nice", STRING_BUFFER_SIZE) == 0)){
      error("kill mode invalid");
    }
  sprintf(message, "%d %s %d", jid, mode, gracePeriod);
  char response[STRING_BUFFER_SIZE];
  writeMessage_ToHost_GetResponse(message, NETWORK_MASTER, response);
  printf("%s\n", response);
}

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

void serverChdir(int sockfd, char *dir){
  //Change the directory
  if (chdir(dir) != 0) {
    writeMessage_ToSocket("Invalid path", sockfd);
  } else {
    writeMessage_ToSocket("Done!", sockfd);
  }
}

void serverStatus(int sockfd){
  writeMessage_ToSocket("Job\t\tHost\t\tCommand\t\tType\t\tstatus\t\tDate\t\tTime\n", sockfd);

  FILE *f;
	char fileName[STRING_BUFFER_SIZE];
	sprintf(fileName, "%s/%s", serverStartingCWD, JOBS_FILENAME);
	if( !(f=fopen(fileName, "r")) ) {
    error("Error opening file");
	}

	char line [STRING_BUFFER_SIZE];
	while ( fgets ( line, STRING_BUFFER_SIZE, f ) ){
		Job j = stringToJob(line);
    char state[16];
    char type[16];
    char buffer[STRING_BUFFER_SIZE * 4];
    switch(j.state){
      case WAITING: strncpy(state, "WAITING", STRING_BUFFER_SIZE); break;
      case RUNNING: strncpy(state, "RUNNING", STRING_BUFFER_SIZE); break;
      case TERMINATED: strncpy(state, "TERMINATED", STRING_BUFFER_SIZE); break;
      case FINISHED: strncpy(state, "FINISHED", STRING_BUFFER_SIZE); break;
    }
    switch(j.type){
      case INTERACTIVE: strncpy(type, "INTERACTIVE", STRING_BUFFER_SIZE); break;
      case BATCH: strncpy(type, "BATCH\t", STRING_BUFFER_SIZE); break;
    }
    char command[STRING_BUFFER_SIZE];
    strncpy(command, j.command, STRING_BUFFER_SIZE);
    command[strlen(command) - 1] = '\0'; // change '\n' to '\0' to not skip line
    sprintf(buffer, "%d\t\t%s\t\t%s\t\t%s\t%s\t%d/%d/%d\t%d:%d:%d\n",
      j.jid,
      j.host,
      command,
      type,
      state,
      j.dateTime.tm_mday, j.dateTime.tm_mon, j.dateTime.tm_year,
      j.dateTime.tm_hour, j.dateTime.tm_min, j.dateTime.tm_sec);
    writeMessage_ToSocket(buffer, sockfd);
	}
	fclose(f);
  close(sockfd);
}

void serverCopyFromClient(int sockfd, char *fileNameOnServer){
  remove(fileNameOnServer);
  int f;
  if( !(f = open(fileNameOnServer, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR) ) ) {
    error("Error creating file");
  }

  // fork child to execute the command
  pid_t pid = fork();
  if (pid == 0) {
    // redirection of stdio
    dup2(f, STDOUT_FILENO);
    continuouslyReadAndPrintFromSocketUntilEnd(sockfd);
    close(f);
    exit(EXIT_SUCCESS);
  }else if (pid > 0) {
    // wait for child until it stops ab/normally
    int status = -1;
    while( !(WIFSIGNALED(status) || WIFEXITED(status)) ){
      waitpid(pid, &status, WUNTRACED);
    }
    close(sockfd);
    close(f);
  } else {
    error("Fork Failed");
  }
}
void serverCopyToClient(int sockfd, char *fileNameOnServer){
  FILE *f;

	if( !(f=fopen(fileNameOnServer, "r")) ) {
    error("Error opening file.");
	}

  char line [STRING_BUFFER_SIZE];
	while ( fgets ( line, STRING_BUFFER_SIZE, f ) ){
    writeMessage_ToSocket(line, sockfd);
  }

  close(sockfd);
  fclose(f);
}

void serverKill(int sockfd, char **args){
  int jid = atoi(args[0]);
  char *mode = args[1];
  int gracePeriod = atoi(args[2]);
  // get job id, see if is valid
  // if job id is valid:
  //    if it is running, send appropriate signals
  //    if it is wating, the remove it from batch processes
  //    set job status to terminated
  //    send appropriate message back to client
}