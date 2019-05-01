/******************************************
* project.c
* Main task
* Author: Monish Nene and Sanika Dongre
* Date created: 03/28/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include "common.h"
#include "light_read.h"
#include "logger.h"
#include "temperature_read.h"
#include "server.h"

/*******************************************
* Macros
*******************************************/

#define PERIOD 1
#define DEBUG 1
#ifndef DEBUG
#define printf(fmt, ...) (0)
#endif

/*******************************************
* Global variables
*******************************************/

uint8_t* msg;
int32_t shm_temp=0,shm_light=0;
uint8_t trigger=1;
uint8_t* shm_ptr;
void* ptr;
sem_t* sem_temp;
sem_t* sem_light;
sem_t* sem_trigger;
sem_t* sem_logger_ready;
int32_t celcius=0,fahrenheit=0,kelvin=0,luminosity=0;
timer_t timerid;
struct sigevent signal_event;
struct itimerspec timer_data;
struct sigaction signal_action;
pthread_t thread_light,thread_temperature,thread_logger,thread_server;
sigset_t mask;

/***********************************************************************
 * temperature_run()
 * param ptr pointer to data passed
 * return ptr pointer to data to be returned
 * @brief run temperature measurement functions
/***********************************************************************/
void* temperature_run(void* ptr)
{
	heartbeat_check[temperature_heart]=1;
	temperature_read();
	pthread_exit(ptr);
}

/***********************************************************************
 * light_run()
 * param ptr pointer to data passed
 * return ptr pointer to data to be returned
 * @brief run light measurement functions
/***********************************************************************/
void* light_run(void* ptr)
{
	heartbeat_check[light_heart]=1;
	light_read();
	pthread_exit(ptr);
}

/***********************************************************************
 * logger_run()
 * @param ptr pointer to data passed
 * @return ptr pointer to data to be returned
 * @brief run logger functions
/***********************************************************************/
void* logger_run(void* ptr)
{
	logger();
	pthread_exit(ptr);
}

/***********************************************************************
 * server_run()
 * @param ptr pointer to data passed
 * @return ptr pointer to data to be returned
 * @brief run server functions
/***********************************************************************/
void* server_run(void* ptr)
{
	remote_server();
	pthread_exit(ptr);
}

/***********************************************************************
 * logfile_setup()
 * @brief create a backup of old log file and create a new one
/***********************************************************************/
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


/***********************************************************************
 * temperature_thread()
 * @brief create and join temperature measurement thread
/***********************************************************************/
void temperature_thread()
{
	int32_t error = pthread_create(&thread_temperature,NULL,temperature_run,NULL);
	if(error)
	{
		log_creator(LOG_ERROR,"Error Creating Temperature Thread");
	}
	//pthread join
	error=pthread_join(thread_temperature,NULL);
	if(error)
	{
		sprintf(msg,"Error Joining Temperature Thread %d",error);
		log_creator(LOG_ERROR,msg);
		bzero(msg,STR_SIZE);
	}
	log_creator(LOG_INFO,"Created and joined new thread for measuring temperature");
}


/***********************************************************************
 * light_thread()
 * @brief create and join light measurement thread
/***********************************************************************/
void light_thread()
{
	int32_t error = pthread_create(&thread_light,NULL,light_run,NULL);
	if(error)
	{
		log_creator(LOG_ERROR,"Error Creating Light Thread");
	}
	error=pthread_join(thread_light,NULL);
	if(error)
	{
		sprintf(msg,"Error Joining light Thread %d",error);
		log_creator(LOG_ERROR,msg);
		bzero(msg,STR_SIZE);
	}
	log_creator(LOG_INFO,"Created and joined new thread for measuring light");
}

/***********************************************************************
 * logger_thread()
 * @brief create and join logger thread
/***********************************************************************/
void logger_thread()
{
	//pthread creation logger
	int32_t error = pthread_create(&thread_logger,NULL,logger_run,NULL);
	if(error)
	{
		printf("Error Creating Logger Thread\n");
		kill(getpid(),SIGINT);
	}
	if(!fork())
	{
		error=pthread_join(thread_logger,NULL);
	}
	if(error)
	{
		printf("Error Joining logger Thread %d\n",error);
		kill(getpid(),SIGINT);
	}
}

/***********************************************************************
 * server_thread()
 * @brief create and join server thread
/***********************************************************************/
void server_thread(void)
{
	//pthread creation server
	printf("Creating server with port address:%d\n",server_port);
	int32_t error=pthread_create(&thread_server,NULL,server_run,NULL);   //error checks
	if(error)
	{
		log_creator(LOG_ERROR,"Error Creating Server Thread");
	}
	if(!fork())
	{
		error=pthread_join(thread_server,NULL);
	}
	if(error)
	{
		sprintf(msg,"Error Joining Server Thread %d",error);
		log_creator(LOG_ERROR,msg);
		bzero(msg,STR_SIZE);
	}
	log_creator(LOG_INFO,"Created and joined new thread for server");
}

/***********************************************************************
 * system_end()
 * @param sig to be handled
 * @brief this function is used to exit smoothly when SIGINT is sent to main process
 /***********************************************************************/
void system_end(int sig)
{
	condition=0;
	logger_condition=0;
	server_condition=0;
	sem_unlink(shm_temp_id);
    	sem_unlink(shm_light_id);
	sem_unlink(trigger_sem_id);
	sem_unlink(led_sem_id);
	sem_unlink(logfile_sem_id);
	sem_unlink(i2c_sem_id);
	sem_unlink(logger_ready_id);
}

/***********************************************************************
 * heartbeat()
 * @brief this function is used check if all threads are running properly and restart any stopped thread
 /***********************************************************************/
static void heartbeat(void)
{
	uint8_t i=0,count=0,dummy='?',reply=0;
	int32_t socket_desc=0,query=0,error=0;
	struct sockaddr_in sock_heartbeat;
	struct timespec timer;
	timer.tv_sec=1;
        setsockopt(query,SOL_SOCKET,SO_SNDTIMEO, (const char*)&timer,sizeof(timer));
	sock_heartbeat.sin_addr.s_addr = INADDR_ANY;
        sock_heartbeat.sin_family = AF_INET;
	sock_heartbeat.sin_port = htons(logger_port);
	query = socket(AF_INET, SOCK_STREAM, 0);
        error = connect(query, (struct sockaddr *)&sock_heartbeat, sizeof(sock_heartbeat));
	error = write(query,&dummy,sizeof(dummy));
	error = read(query,&reply,sizeof(reply));
	if(dummy==reply)
	{
		heartbeat_check[logger_heart]=1;
	}
	close(query);
	reply=0;
	sock_heartbeat.sin_port = htons(server_port);
	query = socket(AF_INET, SOCK_STREAM, 0);
        error = connect(query, (struct sockaddr *)&sock_heartbeat, sizeof(sock_heartbeat));
	error = write(query,&dummy,sizeof(dummy));
	error = read(query,&reply,sizeof(reply));
	if(dummy==reply)
	{
		heartbeat_check[server_heart]=1;
	}
	close(query);
	for(i=0;i<TOTAL_HEARTS;i++)
	{
		if(!heartbeat_check[i])
		{
			switch(i)
			{
				case logger_heart:
				{
					printf("logger thread is dead and will be restarted");
					logger_condition=1;
					logger_port+=2;
					logger_thread();
					break;
				}
				case temperature_heart:
				{
					log_creator(LOG_ERROR,"temperature thread is dead and will be restarted");
					if(!fork())
					{
						temperature_thread();
					}
					break;
				}
				case light_heart:
				{
					log_creator(LOG_ERROR,"light thread is dead and will be restarted");
					if(!fork())
					{
						light_thread();
					}
					break;
				}
				case server_heart:
				{
					log_creator(LOG_ERROR,"server thread is dead and will be restarted");
					server_condition=1;
					server_port+=2;
					server_thread();
					break;
				}
			}
		}
		count+=heartbeat_check[i];
	}
	if(count==TOTAL_HEARTS)
	{
		log_creator(LOG_INFO,"All threads are alive");
	}
	for(i=0;i<TOTAL_HEARTS;i++)
	{
		heartbeat_check[i]=0;
	}
}

/***********************************************************************
 * join_threads()
 * @param sig to be handled
 * @param si signal information
 * @param uc data passed
 * @brief this function is used to run threads when triggered by timer
 /***********************************************************************/
static void join_threads(int sig, siginfo_t *si, void *uc)
{
	temperature_thread();
	light_thread();
	heartbeat();
}

/***********************************************************************
 * timer_init()
 * @return error if any
 * @brief this function is used to setup timer for temperature and light measurement
 /***********************************************************************/
int32_t timer_init(void)
{
	int32_t error=0;
	printf("Timer Init\n");
	// Timer init
	signal_action.sa_flags = SA_SIGINFO;
	signal_action.sa_sigaction = join_threads;//Function to be executed
	sigemptyset(&signal_action.sa_mask);
	error=sigaction(SIGRTMIN, &signal_action, NULL);
    signal_event.sigev_notify = SIGEV_SIGNAL;
    signal_event.sigev_signo = SIGRTMIN;
    signal_event.sigev_value.sival_ptr = &timerid;
    error=timer_create(CLOCK_REALTIME, &signal_event, &timerid);
	/* Start the timer */
    timer_data.it_value.tv_sec = PERIOD;
    timer_data.it_value.tv_nsec = 0;
    timer_data.it_interval.tv_sec = timer_data.it_value.tv_sec;
    timer_data.it_interval.tv_nsec = timer_data.it_value.tv_nsec;
	error=timer_settime(timerid, 0, &timer_data, NULL);
	return error;
}


/***********************************************************************
 * kill_logger()
 * @param sig to be handled
 * @brief this function is used to kill logger thread with USR1
 /***********************************************************************/
void kill_logger(int sig)
{
	log_creator(LOG_ERROR,"USR1 Kill logger command received");
	printf("USR1 Kill logger command received\n");
	pthread_cancel(thread_logger);
	logger_condition=0;
}

/***********************************************************************
 * kill_server()
 * @param sig to be handled
 * @brief this function is used to kill server thread with USR2
 /***********************************************************************/
void kill_server(int sig)
{
	log_creator(LOG_ERROR,"USR2 Kill server command received");
	printf("USR2 Kill server command received\n");
	pthread_cancel(thread_server);
	server_condition=0;
}

/***********************************************************************
 * system_init()
 * @return error if any
 * @brief this function is used to initialize all the resources of the project
 /***********************************************************************/
int32_t system_init(void)
{
	int32_t error=0;
	logger_condition=1;
	server_condition=1;
	condition=1;
	ptr=NULL;
	printf("System Init\n");
	shm_temp=shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
	shm_light=shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
	sem_logger_ready=sem_open(logger_ready_id, O_CREAT, 0644,0);
	sem_open(shm_temp_id, O_CREAT, 0644,1);
	sem_open(led_sem_id, O_CREAT, 0644,1);
	sem_open(shm_light_id, O_CREAT, 0644,1);
	sem_open(logfile_sem_id, O_CREAT, 0644,1);
	sem_open(i2c_sem_id, O_CREAT, 0644,1);
	sem_trigger=sem_open(trigger_sem_id, O_CREAT, 0644,1);
	logfile_setup();
	logger_init();
	led_init();
	temperature_init();
	light_init();
	//Signal Handling
	signal(SIGUSR1,kill_logger);
	signal(SIGUSR2,kill_server);
	signal(SIGINT,system_end);
	error=timer_init();
    return error;
}

/***********************************************************************
 * main()
 * @param argc number of command line arguments passed
 * @param argv command line arguments passed (logfile name)
 * @brief This is the main function for the Project.
 /***********************************************************************/
int32_t main(int32_t argc, uint8_t **argv)
{
	int32_t error=0;
	pid_t process_id = getpid();
	printf("Process ID = %d\n",process_id);
	msg=(uint8_t*)malloc(STR_SIZE);
	ptr=&error;
	if(argc<2)
	{
		printf("%s <logfilename>\n",argv[0]);	 //log file name as command line argument
		return 0;
	}
	logfile=argv[1];
	error=system_init();
	logger_thread();
	sem_wait(sem_logger_ready);
	log_creator(LOG_INFO,"Built in start up tests done");
	server_thread();
	if(process_id!=getpid())
	{
		kill(getpid(),SIGINT);
	}
	error=bist_check();
	while(condition)
	{

	}
	free(msg);
}
