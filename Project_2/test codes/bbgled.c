/*****************************************
* LED task
* Author: Sanika Dongre and Monish Nene
* Date created: 03/25/19
*****************************************/

/************************************
* Includes
*************************************/

#include "bbgled.h"

/************************************
* Global variable
*************************************/

sem_t* sem_led;

/***********************************************************************
 * led_init()
 * @brief This function is used to initialize files and folders for led
***********************************************************************/
void led_init(void)
{
	sem_led = sem_open(led_sem_id,0);
	system(led_init_cmd);
}


/***********************************************************************
 * led_on()
 * @param led to be turned on
 * @brief This function is used to turn on led
***********************************************************************/
void led_on(uint8_t led)
{
	sem_wait(sem_led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w+");
		uint8_t data='1',error=0;
		error=fwrite(&data,1,1,fptr);
		//fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	sem_post(sem_led);
	return;
}

/***********************************************************************
 * led_off()
 * @param led to be turned off
 * @brief This function is used to turn off led
***********************************************************************/
void led_off(uint8_t led)
{
	sem_wait(sem_led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		FILE* fptr=fopen(path,"w+");
		uint8_t data='0',error=0;
		error=fwrite(&data,1,1,fptr);
		//fclose(fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	sem_post(sem_led);
	return;
}

/***********************************************************************
 * led_toggle()
 * @param led to be toggled
 * @brief This function is used to toggle led
***********************************************************************/
void led_toggle(uint8_t led)
{
	FILE* fptr;
	sem_wait(sem_led);
	if(led<LED_COUNT)
	{
		path[pathx]='3'+led;
		fptr=fopen(path,"w+");
		if(fptr==NULL)
		{
			printf("file not found for %s\n",path);
		}
		uint8_t data='1',error=0,prev=0;
		error=fread(&prev,1,1,fptr);
		data=(prev=='0')?'1':'0';
		error=fwrite(&data,1,1,fptr);
	}
	else
	{
		printf("LED index error\n");
	}
	sem_post(sem_led);
}

/********************************************t***************************
 * main()
 * @brief This function is used to test leds
***********************************************************************/
#ifdef LED_TEST
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
#endif
