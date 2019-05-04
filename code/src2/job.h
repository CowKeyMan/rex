#ifndef JOB_H
#define JOB_H

#include <semaphore.h>
#include <time.h>

#include "MyTime.h"
#include "Network.h"

#define JOBS_FILENAME "Jobs.txt" // file stored in master. Keeps record of all jobs
#define STRING_BUFFER_SIZE 256

// state of a job
typedef enum {WAITING, RUNNING, TERMINATED, FINISHED} JobState;
typedef enum {INTERACTIVE, BATCH} Type;

typedef struct{
	int pid; // the actual pid of the process
	int jid; // the job pid on the network
	char host[STRING_BUFFER_SIZE];
	char command[STRING_BUFFER_SIZE];
	Type type;
	JobState state;
	struct tm dateTime;
} Job;

Job *jobs; // batch jobs for this server
Job *nextjob;
int numberOfJobs;
int numberOfBatchJobs;

sem_t batch_jobs_mutex;
sem_t jobs_mutex;

// batch jobs list for each server
void jobs_init(); // initialize semaphore
void* addBatchJob(void *newJob); // insert item and sort, and set next job
void* removeTopJob(); // remove the item at the last
void jobs_finish(); // do at exit

void* addJob(void *newJob); // to the job file in the master // returns the job itself with a updted pid
void* changeJob(void *job); // change a job state int he status file given the job

void *getJob(void *jid); // get a job given its id

#endif // JOB_H
