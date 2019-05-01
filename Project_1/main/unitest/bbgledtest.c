/*********************
*LED test
*Author: Sanika Dongre
*Date created: 03/25/19
***********************/

/************************************
* Includes
*************************************/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/gpio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include "bbgled.h"

/************************************
* led_init function
* Function used for initializing LEDs
*************************************/
void led_init(void)
{
	system(led_init_cmd);
}

/**************************************
* LED ON function to turn on the USR LEDs
* by accessing the gpio pins
****************************************/

void led_on(uint8_t led)
{
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w");
		uint8_t data='1',error=0;
		error=fwrite(&data,1,1,fptr);
		fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	return;
}

/**********************************************
* LED OFF function to turn off LEDs by accessing 
* the GPIO pins that can be accessed by user
************************************************/

void led_off(uint8_t led)
{
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w");
		uint8_t data='0',error=0;
		error=fwrite(&data,1,1,fptr);
		fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	return;
}

/************************************
* led toggle function to toggle leds 
* in a pattern
*************************************/

void led_toggle(uint8_t led)
{
	printf("LED%d Toggle\n",led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w+");
		uint8_t data='1',error=0,prev=0;
		error=fread(&prev,1,1,fptr);
		data=(prev=='0')?'1':'0';
		error=fwrite(&data,1,1,fptr);
		fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	return;
}

/**************************************
* void main: to toggle leds with 
* a specific delay
***************************************/

void main()
{
	uint8_t led=0,i=0;
	led_off(0);	
	led_off(1);		
	led_off(2);		
	led_off(3);
	for(i=0;;i++)
	{
		usleep(1e5);
		led_toggle(led);		
		led=(led==LED_COUNT-1)?0:(led+1);
	}
}
