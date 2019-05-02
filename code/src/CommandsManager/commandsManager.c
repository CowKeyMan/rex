#include "commandsManager.h"

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
