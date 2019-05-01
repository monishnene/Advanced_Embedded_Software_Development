/******************************************
* temperature_read.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/26/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include "temperature_read.h"

/*****************************
* Global variables
* shared mem and semaphores
*****************************/

int32_t shm_temp;
sem_t* sem_temp;
sem_t* sem_i2c;
uint8_t* shm_ptr;

/***********************************************************************
 * i2c_read()
 * @param fd file desciptor for i2c
 * @param regval address to be written
 * @brief This function is used to write data to i2c file
***********************************************************************/
static void i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd, &regval, sizeof(regval))<0)
	{
		perror("write function has been failed");
	}
}

/***********************************************************************
 * i2c_read()
 * @param fd file desciptor for i2c
 * @param buffer to fill data in
 * @param size of the data to be read
 * @brief This function is used to read data from i2c file
***********************************************************************/
static int32_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	return read(fd, buffer, size);
}

/***********************************************************************
 * get_temperature()
 * @return Temperature value read from sensor in Celcius
 * @brief This function is used to read data from i2c for temperature
***********************************************************************/
static int32_t get_temperature(void)
{
	//printf("Temperature Get\n");
	int32_t data=0;
	int32_t error=0,fd=0;
	uint8_t buffer[2];
	sem_wait(sem_i2c);
	//file open
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, TEMP_SLAVE_ADDR);
	//sensor tasks
	i2c_write(fd,TEMP_REG_ADDR);
	error = i2c_read(fd,buffer,sizeof(buffer));
	sem_post(sem_i2c);
	data = (((buffer[0] << 8) | buffer[1]) >> 4)/16.0; // temp data in C
	return data;
}


/***********************************************************************
 * temperature_init()
 * @brief This function is used to initialize temperature measurement
***********************************************************************/
void temperature_init(void)
{
	//printf("Temperature Init\n");
	sem_temp = sem_open(shm_temp_id,0);
	sem_i2c = sem_open(i2c_sem_id,0);
	shm_temp = shmget(temperature_id,LOG_SIZE,0666|IPC_CREAT);
}

/***********************************************************************
 * temperature_read()
 * @brief This function is used to read data from temperature sensor and log it
***********************************************************************/
void temperature_read(void)
{
	uint8_t* msg= (uint8_t*)malloc(STR_SIZE);
	led_toggle(temperature_led);
	//declare variables
	log_t log_data;
	int32_t celcius=0,error=0;
	//data collection
	celcius = get_temperature();
	log_data.data[celcius_id]=celcius;
	clock_gettime(CLOCK_REALTIME,&log_data.timestamp);
	log_data.header=temperature_id;
	log_data.data[fahrenheit_id]=(celcius*1.8)+32.0; // celcuis to Fahrenheit
	log_data.data[kelvin_id]=celcius+273.15; // celcius to kelvin
	//shared memory send
	sem_wait(sem_temp);
	shm_ptr=shmat(shm_temp,(void*)0,0);
	memcpy(shm_ptr,&log_data,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_temp);
	if(celcius<15)
	{
		log_creator(LOG_ERROR,"The Temperature is below 15°C");
		led_off(error_led);
	}
	else
	{
		led_on(error_led);
	}
	sprintf(msg,"Temperature: %d°C, %d°K, %d°F",log_data.data[celcius_id],log_data.data[kelvin_id],log_data.data[fahrenheit_id]);
	log_creator(LOG_DATA,msg);
	free(msg);
	return;
}

