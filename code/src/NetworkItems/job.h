#ifndef JOB_H
#define JOB_H

#include "../MyTime/MyTime.h"
#include "networkOptions.h"

#define JOBS_FILENAME "Jobs.txt" // file stored in master. Keeps record of all jobs
#define STRING_BUFFER_SIZE 256

// state of a job
typedef enum {WAITING, RUNNNING, TERMINATED, FINISHED} JobState;
typedef enum {Interactive, Batch} Type;

Job *batchJobs;
Job *nextjob;
int numberOfJobs = 0;
int numberOfBatchJobs = 0;

sem_t batch_jobs_mutex;
sem_t jobs_mutex;

typedef struct{
	int pid; // the actual pid of the process
	int jid; // the job pid on the network
	char[STRING_BUFFER_SIZE] host;
	char[STRING_BUFFER_SIZE] command;
	Type type;
	JobState state;
	struct mt_time *dateTime;
} Job;

// batch jobs list for each server
void jobs_init(); // initialize semaphore
void* addBatchJob(void *newJob); // insert item and sort, and set next job
void* removeTopJob(); // remove the item at the last
void jobs_finish(); // do at exit

void* addJob(void *newJob); // to the job file in the master
void* getJob(void *jid); // get a particular job given its id

#endif // JOB_H
