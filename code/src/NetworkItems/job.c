#include "job.h"

void jobs_init(){
	sem_init(&jobs_mutex, 0, 1); 
}
void* addJob(void *newJob){
	sem_wait(&jobs_mutex);

	//malloc, sort and set next job

	sem_post(&jobs_mutex);
}
void* removeTopJob(){
	sem_wait(&jobs_mutex);

	sem_post(&jobs_mutex);
}
void jobs_finish(){
	sem_destroy(&jobs_mutex);
}
