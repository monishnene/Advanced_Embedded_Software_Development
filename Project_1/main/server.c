/******************************************
* server.c
* Remote socket task
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

/*******************************************
* Includes
*******************************************/
#include "server.h"

/*******************************************
* Global variables
*******************************************/

int32_t server_temp=0,server_light=0;
sem_t* sem_temp;
sem_t* sem_light;
uint8_t* shm_ptr;

/***********************************************************************
 * find_temperature()
 * @return temperature value read
 * @brief This function is used to read temperature from shared memory
/***********************************************************************/
static int16_t find_temperature (void)
{
	int32_t error=0;
	log_t temp_data;
	sem_wait(sem_temp);
	shm_ptr=shmat(server_temp,(void*)0,0);
	memcpy(&temp_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	return temp_data.data[celcius_id];
}

/***********************************************************************
 * find_luminosity()
 * @return light value read
 * @brief This function is used to read luminosity from shared memory
/***********************************************************************/
static int16_t find_luminosity(void)
{
	int32_t error=0;
	log_t light_data;
    sem_wait(sem_light);
	shm_ptr=shmat(server_light,(void*)0,0);
	memcpy(&light_data,shm_ptr,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
	return light_data.data[luminosity_id];
}

/***********************************************************************
 * remote_server()
 * @brief This function is used to continuously run remote server
***********************************************************************/
void remote_server(void)
{
	int32_t sockfd=0,conn=0, fork_child=1, serverlen=0, size=0, temp=0, light=0;
	uint8_t value=0;
	struct hostent* hostptr;
	struct sockaddr_in server_addr, client_addr;
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	//shared mem
	server_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
	server_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	sem_temp = sem_open(shm_temp_id,0);
	sem_light = sem_open(shm_light_id,0);
	//socket create
	if(sockfd < 0)
	{
		perror("socket creation error\n");
	}
	memset((char*)&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port= htons(server_port);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverlen = sizeof(struct sockaddr_in);
	//socket bind
	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
	{
		perror("socket binding error\n");
	}
	//listen
	if(listen(sockfd,5)<0)
	{
		perror("socket listening error\n");
	}
	log_creator(LOG_INFO,"Server Started and waiting for remote client requests");
	while(condition)
	{
		//connection accept
		conn=accept(sockfd,(struct sockaddr*)&client_addr,&serverlen);
		if(!fork())
		{
			size=recv(conn,&value,sizeof(value),0);
			if(value=='?')
			{
				size=write(conn,&value,sizeof(value));
				break;
			}
			log_creator(LOG_INFO,"Data sent to remote client from server");
			temp=find_temperature();
			light=find_luminosity();
			size=write(conn,&light,sizeof(light)); //send lux value to client
			size=write(conn,&temp,sizeof(temp)); // send temperature value to client
			break;
		}
	}
	//socket close
	close(sockfd);
	return;
}


