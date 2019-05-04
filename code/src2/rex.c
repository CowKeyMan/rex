#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "Network.h"
#include "Helper.h"
#include "StringManipulator.h"

// process and do stuff with the arguments given
void computeArgs(char **args);
bool extractDestination(char *arg);
void extractDestinationHard(char *arg); // exit if could not extract destination
void *extractTime(char *date, char *time, char *ret); //return string containing date and time

char destination[STRING_BUFFER_SIZE];
char parameter[STRING_BUFFER_SIZE];

int main(int argc, char *argv[]){
	computeArgs(argv);
}

void computeArgs(char** args){
	//splitStringBy(line, " ", args, STRING_BUFFER_AMOUNT);

	// if no command is found
	if(! args[1]){
		error("No command found\n");
		return;
	}else{
		if(! args[2]){
			error("No parameter found\n");
			return;
		}
	}

  char responseBuffer[1024];

	if(strncmp("run", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
		clientRun(parameter, responseBuffer, NETWORK_BUFFER_SIZE);
	}else if(strncmp("submit", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
    char dateTime[STRING_BUFFER_SIZE];
    extractTime(args[3], args[4], dateTime);
    //clientSubmit(parameter, dateTime, responseBuffer, NETWORK_BUFFER_SIZE);
	}
  
  
  else if(strncmp("kill", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
	}else if(strncmp("status", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
	}else if(strncmp("copy", args[1], STRING_BUFFER_SIZE) == 0){
		if(extractDestination(args[2])){

		}
		else if(args[3] && extractDestination(args[3])){

		}
	}else if(strncmp("chdir", args[1], STRING_BUFFER_SIZE) == 0){
		extractDestinationHard(args[2]);
	}else{
		error("Command not found\n");
	}
}

bool extractDestination(char *arg){
	char *args[STRING_BUFFER_AMOUNT];
	splitStringBy(arg, ":", args, STRING_BUFFER_SIZE);

	if(args[1] != NULL){
		strncpy(destination, args[0], STRING_BUFFER_SIZE);
		strncpy(parameter, args[1], STRING_BUFFER_SIZE);
		return true;
	}

	return false;
}

void extractDestinationHard(char *arg){
	if(!extractDestination(arg)){
			error("ERROR incorrect use. Please enter address after command\n");
	}
}

void *extractTime(char *date, char *_time, char *ret){
	time_t retTime;
	struct tm *t;
	if(date == NULL || strncmp("now", date, STRING_BUFFER_SIZE) == 0){
		retTime = time(NULL);
		t = localtime(&retTime);
		strftime(ret, STRING_BUFFER_SIZE, "%d/%m/%Y %H:%M:%S", t);
	}else if (date && _time){ // both are not null
		int d,m,y,h,M,s;
		if( sscanf(date, "%d/%d/%d", &d, &m, &y) == 3
		&& sscanf(_time, "%d:%d:%d", &h, &M, &s) == 3){
			sprintf(ret, "%d/%d/%d %d:%d:%d", d, m, y, h, M, s);
		 }else{
			error("Invalid date and time\n");
			exit(EXIT_FAILURE);
		 }
	}else{
		error("Invalid date and time\n");
		exit(EXIT_FAILURE);
	}
}