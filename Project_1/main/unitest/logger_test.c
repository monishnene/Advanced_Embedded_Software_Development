/******************************************
* logger.c logger task
* Author: Monish Nene and Sanika Dongre
* Date created: 03/25/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*****************************
* Global variables
* shared mem and semaphores
*****************************/
#define STR_SIZE 200
#define logger_port 8000
uint8_t* logfile;
uint8_t* str;
sem_t* sem_logfile;
sem_t* sem_logger_ready;
int32_t	shm_light,shm_temp,n,i,condition;
time_t present_time;
struct tm *time_and_date;
int32_t socket_desc;
struct sockaddr_in sock_struct_client,sock_struct_server;
static uint8_t* logtype[]={"LOG_INFO","LOG_DATA","LOG_ERROR"};
static uint8_t logger_ready_id[]="check if logger is ready";
static uint8_t logfile_sem_id[]="sem_logfile";

typedef enum
{
	LOG_INFO=0,
	LOG_DATA=1,
	LOG_ERROR=2,
}logtype_t;

/*************************************
* logger init function
**************************************/
void logger_init(void)
{
	FILE* fptr;
	int32_t error=0;
	str=(uint8_t*)calloc(STR_SIZE,1);
	//printf("Logger Init\n");
      	//sem_getvalue(sem_logfile,&error);
	//printf("sem_logfile = %d\n",error);
	sem_wait(sem_logfile);	
	fptr=fopen(logfile,"w");
	if(fptr==NULL)  //error check
	{
		printf("File opening error\n"); 
	}	
	fclose(fptr);
	condition=1;
	sem_post(sem_logfile);
	sock_struct_server.sin_addr.s_addr = INADDR_ANY;
        sock_struct_server.sin_family = AF_INET;
	sock_struct_server.sin_port = htons(logger_port);
	sock_struct_client.sin_addr.s_addr = INADDR_ANY;
        sock_struct_client.sin_family = AF_INET;
	sock_struct_client.sin_port = htons(logger_port);
}

void log_creator(uint8_t logid, uint8_t* str)
{
	int32_t error=0,sock=0;
	uint8_t* msg = (uint8_t*)malloc(STR_SIZE);
	pid_t process_id=getpid();
	pid_t thread_id=syscall(SYS_gettid);
	struct timespec timestamp;
	clock_gettime(CLOCK_REALTIME,&timestamp);
	sprintf(msg,"[PID:%d][TID:%d][%ld.%ld sec] %s-> %s\n",process_id,thread_id,timestamp.tv_sec,timestamp.tv_nsec,logtype[logid],str);
	sock = socket(AF_INET, SOCK_STREAM, 0);
        error = connect(sock, (struct sockaddr *)&sock_struct_client, sizeof(sock_struct_client));
	error = write(sock,msg,strlen(msg));	
	close(sock);
}

/*****************************
* logger function to log data
******************************/
void logger(void)
{	
	FILE* fptr;
	uint8_t* msg = (uint8_t*)malloc(STR_SIZE);
	int32_t error=0,sock=0,struct_size = sizeof(struct sockaddr_in),size=0;
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    	error=bind(socket_desc,(struct sockaddr *)&sock_struct_server, sizeof(sock_struct_server));
    	listen(socket_desc , 3);
	printf("logger started\n");
	sem_post(sem_logger_ready);
	while(condition)
	{
		//connection accept
		sock = accept(socket_desc,(struct sockaddr*)&sock_struct_server,(socklen_t*)&struct_size);
		if(!fork()) 
		{
			size=read(sock,msg,STR_SIZE); //receive data from log creator
			if(*(msg)=='?')
			{
				size=write(sock,msg,1);
				break;
			}
			sem_wait(sem_logfile);	
			fptr=fopen(logfile,"a");
			n=fwrite(msg,1,size,fptr);
			fclose(fptr);
			sem_post(sem_logfile);	
			break;
		}
	}
	//socket close
	free(msg);
	close(sock);
	return;
}

/*****************************
* log file setup function
********************************/
void logfile_setup(void)
{
	FILE* fptr=fopen(logfile,"r");
	if(fptr==NULL)
	{
		return;
	}
	uint8_t* new_filename=malloc(STR_SIZE);
	uint32_t counter=1;
	while(fptr!=NULL)
	{
		fclose(fptr);
		sprintf(new_filename,"backup_%d_%s",counter++,logfile); //to create backup files
		fptr=fopen(new_filename,"r");	
	}
	rename(logfile,new_filename);
	return;
}

int32_t main(int32_t argc, uint8_t **argv)
{
	if(argc<2)
	{
		printf("%s <logfilename>\n",argv[0]);	 //log file name as command line argument
		return 0;
	}	
	sem_logger_ready=sem_open(logger_ready_id, O_CREAT, 0644,0);	
	sem_logfile=sem_open(logfile_sem_id, O_CREAT, 0644,1);
	logfile=argv[1];
	logfile_setup();
	logger_init();
	if(!fork())
	{
		logger();
	}
	sem_wait(sem_logger_ready);
	for(i=0;i<10;i++)
	{
		log_creator(LOG_INFO,"New test log");
		usleep(1e6);
	}
	condition=0;
	sem_unlink(logfile_sem_id);
	sem_unlink(logger_ready_id);
}
