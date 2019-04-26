#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <string.h>

#include "../linenoise/linenoise.h"
#include "../StringManipulator/StringManipulator.h"
#include "../CommandsManager/commandsManager.h"

#define STRING_BUFFER_SIZE 512
#define STRING_BUFFER_AMOUNT 16

#define CLIENT_PORT 5000
#define SERVER_PORT 5001

void signalHandler(int signalNumber);

//process the given line
void computeLine(char* line);
//set current working directory
void resetCWD();

//Signal handler struct
//struct sigaction sa;

//Used to store the default STDIN, OUT, ERR for reverting them after a command
int stdinFileDescriptor;
int stdoutFileDescriptor;
int stderrFileDescriptor;

char *paths[STRING_BUFFER_SIZE] = {
	"CURRENT WORKING DIRECTORY",
	"/usr/bin",
	"/bin",
	"/usr/local/bin"
};

int main(int argc, char *argv[]){
	resetCWD();

	//Duplicate the default file descriptors
	stdinFileDescriptor = dup(STDIN_FILENO);
	stdoutFileDescriptor = dup(STDOUT_FILENO);
	stderrFileDescriptor = dup(STDERR_FILENO);



}

void computeLine(char* line){
	char *args[STRING_BUFFER_AMOUNT];
	char *rexdArgs[STRING_BUFFER_AMOUNT];
	splitStringBy(line, " ", args, STRING_BUFFER_AMOUNT);

	if(strncmp("exit", args[0], STRING_BUFFER_SIZE) == 0){
		exit(EXIT_SUCCESS);
	}else if(strncmp("chdir", args[0], STRING_BUFFER_SIZE) == 0
			|| strncmp("chdir", args[0], STRING_BUFFER_SIZE) == 0){
		char *newDir = args[1];
		if(changeCWD(newDir)){
			resetCWD();		
		}
	}else if(strncmp("rexl", args[0], STRING_BUFFER_SIZE) == 0){
		//do rex stuff
	}else{
		forkChild(paths, args, STRING_BUFFER_SIZE, true);
	}
}


void resetCWD(){
	char buffer[STRING_BUFFER_SIZE];
	getcwd(buffer, STRING_BUFFER_SIZE);
	paths[0] = (char*)malloc(STRING_BUFFER_SIZE * sizeof(char));
	strncpy(paths[0], buffer, STRING_BUFFER_SIZE);
	printf("CWD: %s\n", paths[0]);
}







/*
int main(){
	char* a[] = {"/bin/ls", NULL};
	char buffer [200];
    getcwd(buffer, 200);
	printf("hey: %s\n", buffer);
	printf("\n\n%d\n\n", execv(a[0], a));

	return 0;
}*/
