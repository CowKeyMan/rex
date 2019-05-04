#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>

#include "job.h"

void jobs_init(){
	sem_init(&batch_jobs_mutex, 0, 1); 
	sem_init(&jobs_mutex, 0, 1); 
}

void* addBatchJob(void *newJob){
	sem_wait(&batch_jobs_mutex);

	numberOfBatchJobs++;
	jobs = (Job*)malloc( numberOfBatchJobs * sizeof(Job) );

	if(numberOfBatchJobs > 1){
		/*if( timeBiggerThan( jobs[numberOfBatchJobs - 2].dateTime, ((Job*)newJob)->dateTime) ){
			jobs[numberOfBatchJobs - 1] = *(Job*)newJob;
		}else{
			jobs[numberOfBatchJobs - 1] = jobs[numberOfBatchJobs - 2];
		}*/
	}else{
		jobs[numberOfBatchJobs - 1] = *(Job*)newJob;
	}

	sem_post(&batch_jobs_mutex);
}

void* removeTopJob(){
	sem_wait(&batch_jobs_mutex);

	numberOfBatchJobs--;
	jobs = (Job*)malloc( numberOfBatchJobs * sizeof(Job) );

	sem_post(&batch_jobs_mutex);
}

void* addJob(void *newJob){
	sem_wait(&jobs_mutex);

	Job *j = (Job*)newJob;
	
	numberOfJobs++;
	j->jid = numberOfJobs;

	FILE *f;

	if( !(f=fopen(JOBS_FILENAME, "a")) ) {
    perror("Error opening file.");
	}

	char newLine[STRING_BUFFER_SIZE];

	char dateTimeString[STRING_BUFFER_SIZE];

	sprintf(dateTimeString, "%d/%d/%d %d:%d:%d", 
			j->dateTime.tm_mday, j->dateTime.tm_mon, j->dateTime.tm_year,
			j->dateTime.tm_hour, j->dateTime.tm_min, j->dateTime.tm_sec);

	char buff[8];
	sprintf(buff, "%d", j->jid);

	strncpy(newLine,buff, STRING_BUFFER_SIZE);
	strncat(newLine, " ", STRING_BUFFER_SIZE);
	strncat(newLine, j->host, STRING_BUFFER_SIZE);
	strncat(newLine, " ", STRING_BUFFER_SIZE);
	strncat(newLine, j->command, STRING_BUFFER_SIZE);
	strncat(newLine, " ", STRING_BUFFER_SIZE);
	switch(j->type){
		case INTERACTIVE:
			strncat(newLine, "I", STRING_BUFFER_SIZE);
		break;
		case BATCH:
			strncat(newLine, "B", STRING_BUFFER_SIZE);
		break;
	}
	strncat(newLine, " ", STRING_BUFFER_SIZE);
	switch(j->state){
		case WAITING:
			strncat(newLine, "WAITING", STRING_BUFFER_SIZE);
		break;
		case RUNNING:
			strncat(newLine, "RUNNING", STRING_BUFFER_SIZE);
		break;
		case TERMINATED:
			strncat(newLine, "TERMINATED", STRING_BUFFER_SIZE);
		break;
		case FINISHED:
			strncat(newLine, "FINISHED", STRING_BUFFER_SIZE);
		break;
	}
	strncat(newLine, " ", STRING_BUFFER_SIZE);
	strncat(newLine, dateTimeString, STRING_BUFFER_SIZE);
	strncat(newLine, "\n", STRING_BUFFER_SIZE);

	fprintf(f, "%s", newLine);

	sem_post(&jobs_mutex);

	return (void*)&j->jid;
}

void* changeJob(void *job){}

void *getJob(void *jid){}

void jobs_finish(){
	sem_destroy(&batch_jobs_mutex);
	sem_destroy(&jobs_mutex);
}

Job createJob(int pid, char *host, char *command, Type type, JobState state, struct tm *dateTime){
	Job j;
	j.pid = pid;
	strncpy(j.host, host, STRING_BUFFER_SIZE);
	strncpy(j.command, command, STRING_BUFFER_SIZE);
	j.type = type;
	j.state = state;
	j.dateTime = *dateTime;

	return j;
}

Job createJob(char *host, char *command, Type type, JobState state, struct tm *dateTime){
	return createJob(0, host, command, type, state, dateTime);
}

Job createJobNow(int pid, char *host, char *command, Type type, JobState state){
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);

	return createJob(pid, host, command, type, state, lt);
}

Job createJobNow(char *host, char *command, Type type, JobState state){
	time_t t = time(NULL);
	struct tm *lt = localtime(&t);

	return createJob(host, command, type, state, lt);
}