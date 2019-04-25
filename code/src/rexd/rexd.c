#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <signal.h>

#include "../linenoise/linenoise.h"
#include "../StringManipulator/StringManipulator.h"

#define STRING_BUFFER_SIZE 256

void signalHandler(int signalNumber);

//Signal handler struct
//struct sigaction sa;

//Used to store the default STDIN, OUT, ERR for reverting them after a command
int stdinFileDescriptor;
int stdoutFileDescriptor;
int stderrFileDescriptor;

char a[][STRING_BUFFER_SIZE] = {
	"CURRENT WORKING DIRECTORY"
	"/usr/bin",
	"/bin",
	"/usr/local/bin"
};

int main(int argc, char *argv[]){
	// open socket and listen to commands
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
