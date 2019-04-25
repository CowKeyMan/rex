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

// process and do stuff withthe arguments given
void computeArgs(char** args);

int main(int argc, char *argv[]){
	computeArgs(argv);
}

void computeArgs(char** args){
	//splitStringBy(line, " ", args, STRING_BUFFER_AMOUNT);

	if(strncmp("run", args[1], STRING_BUFFER_SIZE) == 0){
		
	}else if(strncmp("submit", args[1], STRING_BUFFER_SIZE) == 0){

	}else if(strncmp("kill", args[1], STRING_BUFFER_SIZE) == 0){

	}else if(strncmp("status", args[1], STRING_BUFFER_SIZE) == 0){

	}else if(strncmp("copy", args[1], STRING_BUFFER_SIZE) == 0){

	}else if(strncmp("chdir", args[0], STRING_BUFFER_SIZE) == 0){

	}else{
		fprintf(stderr, "Command no found\n");
	}
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
