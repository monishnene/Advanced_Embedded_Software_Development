/******************************************
* logger.c logger task
* Author: Monish Nene and Sanika Dongre
* Date created: 03/25/19
*******************************************/

/*******************************************
* Includes
*******************************************/


#include "logger.h"

/*****************************
* Global variables
* shared mem and semaphores
*****************************/

uint8_t* str;
int32_t n;
sem_t* sem_logfile;
sem_t* sem_temp;
sem_t* sem_light;
sem_t* sem_logger_ready;
int32_t	shm_light,shm_temp;
time_t present_time;
struct tm *time_and_date;
int32_t socket_desc;
struct sockaddr_in sock_struct_client,sock_struct_server;

/***********************************************************************
 * logger_init()
 * @brief This function is used to intialize the resources required for logger
 /***********************************************************************/
void logger_init(void)
{
	FILE* fptr;
	int32_t error=0;
	str=(uint8_t*)calloc(STR_SIZE,1);
	//printf("Logger Init\n");
	sem_logfile = sem_open(logfile_sem_id, 0);
	sem_temp = sem_open(shm_temp_id,0);
	sem_light = sem_open(shm_light_id,0);
	sem_logger_ready = sem_open(logger_ready_id,0);
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
      	//sem_getvalue(sem_logfile,&error);
	//printf("sem_logfile = %d\n",error);
	sem_wait(sem_logfile);
	fptr=fopen(logfile,"w");
	if(fptr==NULL)  //error check
	{
		printf("File opening error\n");
	}
	fclose(fptr);
	sem_post(sem_logfile);
	sock_struct_server.sin_addr.s_addr = INADDR_ANY;
    sock_struct_server.sin_family = AF_INET;
	sock_struct_server.sin_port = htons(logger_port);
	sock_struct_client.sin_addr.s_addr = INADDR_ANY;
    sock_struct_client.sin_family = AF_INET;
	sock_struct_client.sin_port = htons(logger_port);
}

/***********************************************************************
 * logger_creator()
 * @param logid type of log
 * @param str message to be logged
 * @brief log creator to create log
 /***********************************************************************/
void log_creator(uint8_t logid, uint8_t* str)
{
	int32_t error=0,sock=0;
	uint8_t* msg = (uint8_t*)malloc(STR_SIZE);
	pid_t process_id=getpid();
	pid_t thread_id=syscall(SYS_gettid);
	struct timespec timestamp;
	clock_gettime(CLOCK_REALTIME,&timestamp);
	sprintf(msg,"[PID:%d][TID:%d][%d.%d sec] %s-> %s\n",process_id,thread_id,timestamp.tv_sec,timestamp.tv_nsec,logtype[logid],str);
	sock = socket(AF_INET, SOCK_STREAM, 0);
        error = connect(sock, (struct sockaddr *)&sock_struct_client, sizeof(sock_struct_client));
	error = write(sock,msg,strlen(msg));
	close(sock);
}

/***********************************************************************
 * logger()
 * @brief logger to run continuously and log data
 /***********************************************************************/
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
			led_toggle(logger_led);
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
