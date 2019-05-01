/******************************************
* light_read.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/26/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include "light_read.h"

/*****************************
* Global variables
* shared mem and semaphores
*****************************/
int32_t shm_light;
sem_t* sem_light;
sem_t* sem_i2c;
uint8_t* shm_ptr;

/***********************************************************************
 * i2c_write()
 * @param fd file descriptor
 * @param regval register to be written
 * @brief This function is used to write data to i2c file
 /***********************************************************************/
static void i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd,&regval,1)<0)
	{
		perror("write function has been failed");
	}
}

/***********************************************************************
 * i2c_read()
 * @param fd file descriptor
 * @param buffer to save data
 * @param size of buffer
 * @return size of data read
 * @brief This function is used to read data from i2c file
 /***********************************************************************/
static int32_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	return read(fd, buffer, size);
}

/***********************************************************************
 * get_luminosity()
 * @return light value read from the sensor
 * @brief This function is used to read luminosity from sensor and save  it in shared memory
/***********************************************************************/
float get_luminosity()
{
	uint8_t sensor_id=0, powerval=0, timer=0;
	int32_t error=0,fd=0;
	uint8_t databuff=1, dataop;
	uint8_t regval;
	uint16_t ch0_l=0,ch1_l=0,ch0_h=0,ch1_h=0;
	uint16_t ch0=0,ch1=0;
	float adcval=0.0;
	int16_t lux_output=0;
	sem_wait(sem_i2c);
	//i2c init
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);
	//power on
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
	error=i2c_read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		//printf("the value of power is %x\n", powerval);
	}
	//read channels
	uint8_t addr = 0x8C;
	if(write(fd,&addr,1)!=1)
	{
		perror("errror in write ch0l\n");
	}
	if(read(fd,&ch0_l,1)!=1)
	{
		perror("error in read ch0l\n");
	}
	addr=0x8D;
	if(write(fd,&addr,1)!=1)
	{
		perror("error in write ch0h\n");
	}
	if(read(fd,&ch0_h,1)!=1)
	{
		perror("error in read choh\n");
	}
	addr=0x8E;
	if(write(fd,&addr,1)!=1)
	{
		perror("error in write ch1l\n");
	}
	if(read(fd,&ch1_l,1)!=1)
	{
		perror("error in read ch1li\n");
	}
	addr=0x8F;
	if(write(fd,&addr,1)!=1)
	{
		perror("error in write ch1h\n");
	}
	if(read(fd,&ch1_h,1)!=1)
	{
		perror("error in read ch1h\n");
	}
	sem_post(sem_i2c);
	ch1=(ch1_h<<8)|ch1_l;
	ch0=(ch0_h<<8)|ch0_l;
	adcval = (float)ch1/(float)ch0;
	//check adc range
	if(adcval>0 && adcval <= 0.5)
	{
		lux_output = (0.0304 * ch0) - (0.062 * ch0 * pow(adcval, 1.4));
	}
	else if(adcval<0.61)
	{
		lux_output = (0.0224 * ch0) - (0.031 * ch1);
	}
	else if(adcval<0.80)
	{
        	lux_output = (0.0128 * ch0) - (0.0153 * ch1);
	}
	else if(adcval<1.30)
	{
        	lux_output = (0.00146 * ch0) - (0.00112 * ch1);
	}
    	else
	{
		lux_output=0;
	}
	return lux_output;
}

/***********************************************************************
 * light_init()
 * @brief This function is used to initializing the resources required for light measurement
/***********************************************************************/
void light_init(void)
{
	sem_light = sem_open(shm_light_id,0);
	sem_i2c = sem_open(i2c_sem_id,0);
	shm_light = shmget(luminosity_id,LOG_SIZE,0666|IPC_CREAT);
}

/***********************************************************************
 * light_read()
 * @brief This function is used to read luminosity from sensor and log it
/***********************************************************************/
void light_read(void)
{
	int32_t error=0;
	uint8_t* msg= (uint8_t*)malloc(STR_SIZE);
	//printf("Light Read\n");
	led_toggle(light_led);
	//declare variables
	log_t log_data;
	//data collection
	log_data.data[luminosity_id]=(int16_t)get_luminosity();
	clock_gettime(CLOCK_REALTIME,&log_data.timestamp);
	log_data.header=light_id;
	//shared memory send
	sem_wait(sem_light);
	shm_ptr=shmat(shm_light,(void*)0,0);
	memcpy(shm_ptr,&log_data,LOG_SIZE);
	shmdt(shm_ptr);
	sem_post(sem_light);
	sprintf(msg,"Luminosity: %d",log_data.data[luminosity_id]);
	log_creator(LOG_DATA,msg);
	//printf("Light Read Done\n");
	free(msg);
	return;
}

