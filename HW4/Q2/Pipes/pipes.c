#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <semaphore.h>
#include <syscall.h>
#include <sys/wait.h>
#include <fcntl.h>
#define STR_SIZE 200
#define LED_SIGNAL 1
#define STR_SIGNAL 2
#define TOTAL_MESSAGES 20
#define BUFFER_SIZE 100
#define TOTAL_ANIMALS 12

sem_t* sem_logfile;
sem_t* sem_send_receive[2];
_Bool chance=0;
struct timespec accutime,timer;
time_t present_time;
struct tm *time_and_date;
uint8_t led = 1;
uint8_t IPC[]="Pipes";
uint8_t* animals[TOTAL_ANIMALS]={"Tiger","Zebra","Lion","Giraffe","Rhino","Bear","Panda","Deer",
		"Cheetah","Wolf","Hippo","Elephant"};
uint8_t* process_name[2]={"Child","Parent"};

void send_data(uint8_t* buffer,uint32_t size)
{
	
}

uint32_t receive_data(uint8_t* buffer)
{
	return 0;
}

void first_log(uint8_t* filename)
{
	uint8_t* str=(uint8_t*)calloc(STR_SIZE,1);
	uint32_t n=0,customer_id=0,transmission_id=0;
	pid_t process_id=getpid();
	sem_wait(sem_logfile);
	FILE* fptr=fopen(filename,"a");
	present_time=time(NULL);
	time_and_date = localtime(&present_time);
	sprintf(str,"\nTime & Date : %sProcess Id : %d Process:%s\tIPC method : %s\tFile Descriptor : %d\n",asctime(time_and_date),process_id,process_name[chance],IPC,fileno(fptr));
	n=fwrite(str,1,strlen(str),fptr);
	fclose(fptr);
	sem_post(sem_logfile);
	free(str);
}

void log_event(uint8_t* filename,uint8_t* msg)
{
	uint8_t* str=(uint8_t*)calloc(STR_SIZE,1);
	uint32_t n=0,customer_id=0;;
	pid_t process_id=getpid();
	sem_wait(sem_logfile);
	FILE* fptr=fopen(filename,"a");
	present_time=time(NULL);
	time_and_date = localtime(&present_time);
	sprintf(str,"\nTime & Date : %sProcess Id : %d Process:%s\n%s\n",asctime(time_and_date),process_id,process_name[chance],msg);
	n=fwrite(str,1,strlen(str),fptr);
	fclose(fptr);
	sem_post(sem_logfile);
	free(str);
}

int32_t main(int32_t argc, uint8_t **argv)
{
	uint8_t* filename;
	int32_t error=0,transmission_id=0;
	uint32_t i=0,random=0,size=0;
	uint8_t* buffer=(uint8_t*)calloc(BUFFER_SIZE,1);
	uint8_t* msg=(uint8_t*)calloc(BUFFER_SIZE,1);
	srand(time(NULL));
	sem_logfile = sem_open("/sem_logfile", O_CREAT, 0644, 1);
	sem_send_receive[0] = sem_open("/sem_send_receive1", O_CREAT, 0644, 1);
	sem_send_receive[1] = sem_open("/sem_send_receive2", O_CREAT, 0644, 1);
	if(argc==1)
	{
		printf("Format:%s <filename> \n",*argv);
		kill(getpid(),SIGINT);
	}
	else
	{
		filename=*(argv+1);
	}
	error=remove(filename);
	if(fork())
	{
		chance=1;
	}
	first_log(filename);
	for(i=0;i<TOTAL_MESSAGES;i++)
	{
		size=6;
		if(i%2==chance)
		{
			//send			
			sem_wait(sem_send_receive[chance]);
			clock_gettime(CLOCK_REALTIME,&accutime);	
			srand(accutime.tv_nsec);	
			transmission_id=rand();
			random=transmission_id%2;
			if(random)
			{
				led=rand()%2;
				*(buffer)=LED_SIGNAL;				
				*((uint32_t*)(buffer+1))=transmission_id;
				*(buffer+5)=led;
				send_data(buffer,size);
				sprintf(msg,"Transmission ID: %d, Sent LED : %d",transmission_id,led);
				log_event(filename,msg);
			}
			else
			{
				*(buffer)=STR_SIGNAL;	
				*((uint32_t*)(buffer+1))=transmission_id;
				sprintf(buffer+5,"Next Animal in the ecosystem is %s",animals[rand()%TOTAL_ANIMALS]);
				send_data(buffer,size+strlen(buffer+5));
				sprintf(msg,"Transmission ID: %d, Sent STR : %s",transmission_id,buffer+5);
				log_event(filename,msg);
			}
		}
		else
		{
			size=receive_data(buffer);
			transmission_id=*((uint32_t*)(buffer+1));
			if(*(buffer)==LED_SIGNAL)
			{
				led=*(buffer+5);
				sprintf(msg,"Transmission ID: %d, Received LED : %d",transmission_id,led);
				log_event(filename,msg);
			}
			else if (*(buffer)==STR_SIGNAL)
			{
				sprintf(msg,"Transmission ID: %d, Received STR : %s",transmission_id,buffer+5);
				log_event(filename,msg);
			}
			else
			{
				sprintf(msg,"Transmission ID: %d, Unrecognized format of received data",transmission_id);
				log_event(filename,msg);	
			}
			sem_post(sem_send_receive[!chance]);
		}
		bzero(msg,BUFFER_SIZE);
		bzero(buffer,BUFFER_SIZE);
		size=0;
	}
	// exit
	free(msg);
	free(buffer);
	if(chance)
	{
    		sem_close(sem_logfile);
		sem_close(sem_send_receive[0]);	
		sem_close(sem_send_receive[1]);
    		sem_unlink("/sem_logfile");
    		sem_unlink("/sem_send_receive1");	
    		sem_unlink("/sem_send_receive2");\
	}
    	return 0;
}
