#include "job.h"
#include <string.h>

void jobs_init(){
	sem_init(&batch_jobs_mutex, 0, 1); 
	sem_init(&jobs_mutex, 0, 1); 
}

void* addBatchJob(void *newJob){
	sem_wait(&batch_jobs_mutex);

	numberOfBatchJobs++;
	batchJobs = (Job*)malloc( numberOfBatchJobs * sizeof(Job) );

	if(numberOfBatchJobs > 1){
		if( timeBiggerThan( jobs[numberOfBatchJobs - 2].dateTime, (Job*)newJob->dateTime) ){
			jobs[numberOfBatchJobs - 1] = *(Job*)newJob;
		}else{
			jobs[numberOfBatchJobs - 1] = jobs[numberOfBatchJobs - 2];
		}
	}else{
		jobs[numberOfBatchJobs - 1] = *(Job*)newJob;
	}

	sem_post(&batch_jobs_mutex);
}

void* removeTopJob(){
	sem_wait(&batch_jobs_mutex);

	numberOfBatchJobs--;
	batchJobs = (Job*)malloc( numberOfBatchJobs * sizeof(Job) );

	sem_post(&batch_jobs_mutex);
}

void* addJob(void *newJob){
	sem_wait(&jobs_mutex);

	Job *j = (Job*)newJob;
	FILE *f;

	if( !(f=fopen(JOBS_FILENAME, "a")) ) {
    perror("Error opening file.");
	}

	char[STRING_BUFFER_SIZE] newLine;

	char[STRING_BUFFER_SIZE] dateTimeString;

	sprintf(dateTimeString, "%d/%d/%d %d:%d:%d", 
			j->dateTime->tm_mday, j->dateTime->tm_mon, j->dateTime->tm_year,
			j->dateTime->tm_hour, j->dateTime->tm_min, j->dateTime->tm_sec);

	strcpy(newLine, j->jid);
	strcat(newLine, " ");
	strcpy(newLine, j->host);
	strcat(newLine, " ");
	strcpy(newLine, j->command);
	strcat(newLine, " ");
	strcpy(newLine, j->type);
	strcat(newLine, " ");
	strcpy(newLine, j->status);
	strcat(newLine, " ");
	strcpy(newLine, dateTimeString);
	strcat(newLine, "\n");

	fprintf(f, "%s", newLine);

	sem_post(&jobs_mutex);
}

void* getJob(void *jid){}

void jobs_finish(){
	sem_destroy(&batch_jobs_mutex);
	sem_destroy(&jobs_mutex);
}
