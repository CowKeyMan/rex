#ifndef JOB_H
#define JOB_H

#include "../MyTime/MyTime.h"

// state of a job
typedef enum {WAITING, RUNNNING, TERMINATED, FINISHED} JobState;

Job *jobs;
Job *nextjob;
int numberOFJobs;

sem_t jobs_mutex;

typedef struct{
	int jid; // the job pid
	int pid; // the actual pid of the process
	JobState state;
} Job;

void jobs_init(); // initialize semaphore
void* addJob(void *newJob); // insert item and sort, and set next job
void* removeTopJob(); // remove the item at the last
void jobs_finish(); // do at exit
#endif // JOB_H
