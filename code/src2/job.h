#ifndef JOB_H
#define JOB_H

#include <semaphore.h>
#include <time.h>

#include "MyTime.h"
#include "Network.h"

#define JOBS_FILENAME "Jobs/Jobs.txt" // file stored in master. Keeps record of all jobs
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
void addBatchJob(Job *newJob); // insert item and sort, and set next job
int getNoOfBatchJobs(); // safely get no of batch jobs
void removeTopJob(); // remove the item at the last
void removeJob(int jid);
void jobs_finish(); // do at exit

void addJob(Job *newJob); // to the job file in the master // returns the job itself with a updted pid
void changeJob(Job *job); // change a job state int he status file given the job
Job *getJob(int jid); // get a job given its ID, returns null if not found

Job createJobPid(int pid, char *host, char *command, Type type, JobState state, struct tm *dateTime);
Job createJob(char *host, char *command, Type type, JobState state, struct tm *dateTime);
Job createJobNowPid(int pid, char *host, char *command, Type type, JobState state);
Job createJobNow(char *host, char *command, Type type, JobState state);

// read and put command as the last thing to avoid confusion
Job stringToJob(char *string);
void jobToString(Job *, char *stringBuffer);

#endif // JOB_H
