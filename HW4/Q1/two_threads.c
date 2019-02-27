#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#define RUNTIME 1

uint8_t** filename;
struct timespec start_time={0,0};
struct timespec current_time={0,0};
sem_t sem_thread2;
sem_t sem_logfile;

void* analyze_file(void* ptr)
{
	printf("thread 1: Filename = %s\n",*filename);
	return (void *)ptr;
}

void* cpu_util(void* ptr)
{
	clock_gettime(CLOCK_REALTIME,&current_time);
	sem_post(&sem_thread2);
	while(current_time.tv_sec - start_time.tv_sec < RUNTIME)
	{	
		sem_wait(&sem_thread2);
		printf("thread 2\n");
		clock_gettime(CLOCK_REALTIME,&current_time);
		sem_post(&sem_thread2);
	}
	return (void *)ptr;
}

void run_thread2(void)
{
	sem_post(&sem_thread2);
}

int32_t main(int32_t argc, uint8_t **argv)
{
	if(argc==1)
	{
		printf("Format:%s <filename> \n",*argv);
		kill(getpid(),SIGINT);
	}
	else
	{
		filename=argv+1;
	}
	pthread_t thread_1,thread_2;
	int32_t error=0;
	uint8_t dummy=0;	
	void* ptr=&dummy;
	clock_gettime(CLOCK_REALTIME,&start_time);
	error = pthread_create(&thread_1, NULL, analyze_file, (void*)ptr);
	if(error)
	{
		printf("Error Creating Thread 1\n");
	}
	error = pthread_create(&thread_2, NULL, cpu_util, (void*)ptr);
	if(error)
	{
		printf("Error Creating Thread 2\n");
	}
	//error=timer_create	
	pthread_join(thread_1,&ptr);
	pthread_join(thread_2,&ptr);
	kill(getpid(),SIGINT);
	return 0;
}
