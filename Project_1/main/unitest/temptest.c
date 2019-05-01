/******************************************
* tempsensor.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/25/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <time.h>

/*******************************************
* Macros
*******************************************/

#define SLAVE     (0x48)
#define TEMPREG    (00)
#define INITIAL        (0x60A0)
#define CONFIG  (0x01)
#define TLOW    (0x02)
#define THIGH   (0x03)
#define HIGHMASK       (0x00FF)
#define INTTERUPT         (0x62a0)
#define SHUTDOWN       (0x61a0)

/*******************************************
* Global variables
*******************************************/
int val;
int16_t celcius=0,kelvin=0,fahrenheit=0;


typedef enum  //error or success enum
{
	error=0,
	success=1
}error_check;


/***********************
*temp_file function
************************/

uint8_t temp_file_func()
{
	int output;
	output= open("/dev/i2c-2", O_RDWR);
	if(output<0)
	{	
		perror("file open failed\n");
		return error;
	}
	if(ioctl(output, I2C_SLAVE, SLAVE)<0)
	{
		return error;
	}
	return success;
}

/********************
* Write operation 
***********************/
uint8_t i2c_write(int32_t temp_fd,uint8_t regval)   //used for writing pointer reg
{
	if(write(temp_fd, &regval, sizeof(regval))!=sizeof(regval))
	{
		return error;
	}
	return success;
}

float resolution_set_func(int temp_fd, int data_in)
{
	float data_resolution=0;
	uint8_t* buffer=malloc(sizeof(uint8_t)*2);
	if(data_in==1)
	{
		data_resolution = 0.0625;

	}
	if(data_in==2)
	{
		data_resolution=0.5;
	}
	printf("resolution set is %f\n",data_resolution);
	return data_resolution;
}

uint16_t register_read(int temp_fd, uint8_t regval)
{
	uint8_t buffer[2];
	uint16_t data;
	write(temp_fd,&regval,1);
	read(temp_fd,&buffer,2);
	data=buffer[0];
	data=data<<8;
	data|=buffer[1];
	return data;
}

void register_write(int temp_fd, uint8_t regval, uint16_t data)
{
	uint8_t* buffer=malloc(sizeof(uint8_t)*3);
	buffer[0]=regval;
	*((uint16_t*)(buffer+1))=data;
	write(temp_fd,buffer, 3);
	free(buffer);
}
		
/***************************
* configuration reg write
****************************/
uint16_t configreg_test(int temp_fd)
{
	uint16_t output;
	register_write(temp_fd,CONFIG,4096);
	output=register_read(temp_fd,CONFIG);
	if(output<0)
	{
		return error;
	}
	return success;
}

uint16_t configreg_INTTERUPTmode(int temp_fd)
{
	uint16_t output;
	register_write(temp_fd,CONFIG,INITIAL|INTTERUPT);
	output=register_read(temp_fd,CONFIG);
	if(output==INITIAL|INTTERUPT)
	{
		return success;
	}
	return error;
}

uint16_t configreg_shutdown(int temp_fd)
{
	uint16_t output;
	register_write(temp_fd,CONFIG,INITIAL|SHUTDOWN);
	output=register_read(temp_fd,CONFIG);
	if(output==INITIAL|SHUTDOWN)
	{
		return success;
	}
	return error;
}

uint16_t tlowreg_write(int temp_fd)
{
	uint16_t output;
	uint16_t data=75;
	register_write(temp_fd,TLOW,data);
	output= register_read(temp_fd,TLOW);
	if(output<0)
	{
		return error;
	}
	return success;
}

uint16_t thighreg_write(int temp_fd)
{
	uint16_t output;
	uint16_t data=80;
	register_write(temp_fd,THIGH,data);
	output = register_read(temp_fd,THIGH);
	if(output<0)
	{
		return error;
	}
	return success;
}

/********************************************************
* Get temperature function
* Reads tempreg
* and obtained temp in celsius is output/16
* return the temperature value
*****************************************************/	
	
int get_temperature(int temp_fd)
{
	int data, h_bit=0;
	uint8_t buffer[2], valmsb, vallsb;
	uint8_t addr= TEMPREG;
	write(temp_fd,&addr,1);
	val = read(temp_fd, &buffer, sizeof(buffer));
	valmsb = buffer[0];
	vallsb = buffer[1];
	data = ((valmsb << 8) | vallsb) >> 4; //12 bits resolution
	if(h_bit!=0)
	{	
		return data;
	}
	else 
	{
		data = data/16;
		return data;
	}
}


uint16_t data_check(int temp_fd)
{
	int data = get_temperature(temp_fd);
	if(data<20)
	{
		return success;
	}
	return error;
}

void main()
{
	uint16_t regval, buffer, op;
	uint16_t tester, tester1, tester2, tester3, tester4,tester5;
	int temp_fd;
	op = temp_file_func();
	if(op==0)
	{	
		perror("init failed\n");
	}
	printf("temp sensor is ready\n");
	tester = configreg_test(temp_fd);
	if(tester==1)
	{
		printf("config reg test successful\n");
	}
	else
	{
		printf("config reg test failed\n");
	}
	tester1= configreg_INTTERUPTmode(temp_fd);
	if(tester1==1)
	{
		printf("config reg INTTERUPT test successful\n");
	}
	else
	{
		printf("config reg INTTERUPT test failed\n");
	}
	tester2=configreg_shutdown(temp_fd);
	if(tester2==1)
	{
		printf("config reg shutdown test successful\n");
	}
	else
	{
		printf("config reg shutdown test failed\n");
	}
	tester3=tlowreg_write(temp_fd);
	if(tester3==1)
	{
		printf("tlow reg test successful\n");
	}
	else
	{
		printf("tlow reg test failed\n");
	}
	tester4=thighreg_write(temp_fd);
	if(tester4==1)
	{
		printf("thigh reg test successful\n");
	}
	else
	{
		printf("thigh reg test failed\n");
	}
	if(tester5==1)
	{
		printf("temperature within control\n");
	}
	else
	{
		printf("temperature out of control\n");
	}
	celcius = get_temperature(temp_fd);
	fahrenheit=celcius*1.8+32; // celcius to Fahrenheit
	kelvin=celcius+273.15; // celcius to kelvin
	printf("\ntemperature value in celcius is %d\n", celcius); //temp data in C
	printf("\ntemperature value in fahrenheit is %d\n", fahrenheit); // temp data in F
	printf("\ntemperature value in kelvin is %d\n", kelvin); // temp data in K
}
