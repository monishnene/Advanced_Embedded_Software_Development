#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <semaphore.h>
#define RUNTIME 1
#define PERIOD 100//ms
#define SECTOMSEC 1000
#define MSECTONSEC 1000000

struct timespec start_time={0,0};
struct timespec current_time={0,0};
sem_t sem_thread2;
sem_t sem_logfile;

void* analyze_file(void* ptr)
{
	printf("thread 1: Filename = %s\n",(uint8_t*)ptr);
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

void run_thread2(int sig, siginfo_t *si, void *uc)
{
	sem_post(&sem_thread2);
}

int32_t main(int32_t argc, uint8_t **argv)
{
	pthread_t thread_1,thread_2;
	int32_t error=0;
	uint8_t dummy=0;	
	void* ptr=&dummy;
	uint8_t* filename;
	timer_t timerid;
	struct sigevent signal_event;
	struct itimerspec timer_data;
	struct sigaction signal_action;
	sigset_t mask;
	if(argc==1)
	{
		printf("Format:%s <filename> \n",*argv);
		kill(getpid(),SIGINT);
	}
	else
	{
		filename=*(argv+1);
	}
	// Timer init	
	signal_action.sa_flags = SA_SIGINFO;
	signal_action.sa_sigaction = run_thread2;//Function to be executed
	sigemptyset(&signal_action.sa_mask);
	if (sigaction(SIGRTMIN, &signal_action, NULL) == -1)
        {
		perror("sigaction");
		kill(getpid(),SIGINT);
	}
	sigemptyset(&mask);
        sigaddset(&mask, SIGRTMIN);
        if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
	{        
		perror("sigprocmask");
		kill(getpid(),SIGINT);
	}
        signal_event.sigev_notify = SIGEV_SIGNAL;
        signal_event.sigev_signo = SIG_SETMASK;
        signal_event.sigev_value.sival_ptr = &timerid;
        if (timer_create(CLOCK_REALTIME, &signal_event, &timerid) == -1)
	{        
		perror("timer_create");
		kill(getpid(),SIGINT);
	}
        printf("timer ID is 0x%lx\n", (long) timerid);
	/* Start the timer */
        timer_data.it_value.tv_sec = PERIOD % SECTOMSEC;
        timer_data.it_value.tv_nsec = PERIOD * MSECTONSEC;
        timer_data.it_interval.tv_sec = timer_data.it_value.tv_sec;
        timer_data.it_interval.tv_nsec = timer_data.it_value.tv_nsec;
	if (timer_settime(timerid, 0, &timer_data, NULL) == -1)
	{	
        	perror("timer_settime");
		kill(getpid(),SIGINT);
	}
	// Timer init done
	// Create Threads

	error = pthread_create(&thread_1, NULL, analyze_file, (void*)filename);
	if(error)
	{
		perror("Error Creating Thread 1\n");
		kill(getpid(),SIGINT);
	}
	error = pthread_create(&thread_2, NULL, cpu_util, (void*)filename);
	if(error)
	{
		perror("Error Creating Thread 2\n");
		kill(getpid(),SIGINT);
	}
	
	//Start threads	

	clock_gettime(CLOCK_REALTIME,&start_time);
	pthread_join(thread_1,&ptr);
	pthread_join(thread_2,&ptr);

	// exit process

	kill(getpid(),SIGINT);
	return 0;
}

/*References-
http://man7.org/linux/man-pages/man2/timer_create.2.html

*/
