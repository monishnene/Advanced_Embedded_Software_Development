/******************************************
* light_test.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

/*******************************************
* Includes
*******************************************/

#include "common.h"
#include <math.h>

/*******************************************
* Macros
*******************************************/

#define ID_VALUE (0x50)
#define ID_REGISTER (0x8A)
#define ID_VAL (0X07)
#define LUX_SLAVE_ADDR (0x39)
#define POWER_ADDR (0x80)
#define TIMING_REG (0X81)
#define TIMING_VAL (0X12)
#define START_COMMAND (0X80)
#define POWER_ON_CMD (0x3)
#define CONTROL_VAL (0X09)
#define TLL (0x82)
#define TLH (0x83)
#define THL (0X84)
#define THH (0X85)
#define INTERRUPT_REG (0X86)
#define INTERRUPT_VALUE (0X05)
#define POWER_OFF_CMD (0x00)
#define CH0_L (0x8C)
#define CH0_H (0x8D)
#define CH1_L (0x8E)
#define CH1_H (0x8F)

typedef enum //error or success enum
{
	error=0,
	success=1
}error_check;

/***********************
*i2c_file function
************************/

uint8_t i2c_file(int32_t fd)
{
	fd=open("/dev/i2c-2", O_RDWR);
	if (fd<0)
	{
		return error;
	}
	if(ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR)<0)
	{
		return error;
	}
	return success;
}

/********************
* Write operation 
***********************/
uint8_t i2c_write(int32_t fd,uint8_t regval)
{
	if(write(fd, &regval, sizeof(regval))!=sizeof(regval))
	{
		return error;
	}
	return success;
}

/***************************
* Read operation
****************************/

uint8_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size)
{
	if(read(fd, buffer, size)!=size)
	{
		return error;
	}
	return success;

}

/*************************
* command register test
****************************/

uint8_t cmdreg_write_test(int32_t fd)
{
	uint8_t comm=START_COMMAND; //sending stard command = 80
	if(write(fd, &comm, 1) < 0)
	{
		return error;
	}
	return success;
}

/******************************************
* control register test
* To write and read the control register
******************************************/

uint8_t control_reg_test(int32_t fd)
{
	uint8_t comm=POWER_ADDR;
	int32_t check=0;
	uint8_t value;
	write(fd,&comm,1);
	comm=CONTROL_VAL;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=CONTROL_VAL)
	{
		return error;
	}
	return success;
}

/************************************************
* identification register test
* To write and read the identification register
************************************************/

uint8_t id_reg_test(int32_t fd)
{
	uint8_t comm=ID_REGISTER;
	uint8_t value;
	int32_t check=0;
	write(fd,&comm,1);
	comm=ID_VAL;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=ID_VAL)
	{
		return error;
	}
	return success;
}

/******************************************
* timing register test
* To write and read the timing register
******************************************/

uint8_t timing_reg_test(int32_t fd)
{
	uint8_t value;
	uint8_t check=0;
	uint8_t comm=TIMING_REG;
	write(fd,&comm,1);
	comm=TIMING_VAL;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=TIMING_VAL)
	{
		return error;
	}
	return success;
}

/******************************************
* interrupt control register test
* To write and read the  interrupt control register
******************************************/

uint8_t int_control_reg_test(int32_t fd)
{
	int32_t check=0;
	uint8_t value;
	uint8_t comm=INTERRUPT_REG;
	write(fd,&comm,1);
	comm=INTERRUPT_VALUE;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=INTERRUPT_VALUE)
	{
		return error;
	}
	return success;
}
/******************************************************
* interrupt threshold register test
* To write and read the interrupt threshold register
*******************************************************/

uint8_t int_threshold_test(int fd)
{
	uint8_t dataop=0;
	uint8_t databuff=1;
	int32_t error=0;
	i2c_write(fd,TLL);
	i2c_write(fd,databuff);
	error=i2c_read(fd,&dataop,1);
	if(dataop!=1)
	{
		return error;
	}
	i2c_write(fd,TLH);
	databuff=2;
	i2c_write(fd, databuff);
	error=i2c_read(fd,&dataop,1);
	if(dataop!=2)
	{
		return error;
	}
	i2c_write(fd,THL);
	databuff=3;
	i2c_write(fd,databuff);
	error=i2c_read(fd,&dataop,1);
	if(dataop!=3)
	{
		return error;
	}
	i2c_write(fd,THH);
	databuff=4;
	i2c_write(fd,databuff);
	error=i2c_read(fd,&dataop,1);
	if(dataop!=4)
	{
		return error;
	}
	return success;
}

/******************************************
* power off function
******************************************/

uint8_t poweroff_func(int32_t fd)
{
	uint8_t value=3;
	int32_t check=0;
	uint8_t comm=POWER_ADDR;
	write(fd,&comm,1);
	comm=POWER_OFF_CMD;
	write(fd,&comm,1);
	check=read(fd,&value,1);
	if(value!=0)
	{
		return error;
	}
	return success;	
}

/******************************************************
* Get luminosity function
* Reads data registers (0 and 1)
* and then lux output is calculated based on formula
* return the lux value in float
*********************************************************/

float get_luminosity(int32_t fd)
{
	uint8_t sensor_id=0, powerval=0, timer=0;
	int32_t error=0;
	uint8_t databuff=1, dataop;
	uint8_t regval;
	uint16_t ch0_l=0,ch1_l=0,ch0_h=0,ch1_h=0;
	uint16_t ch0=0,ch1=0;
	float adcval=0.0;
	int16_t lux_output=0;
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
	printf("ch0l=%d,ch1l=%d, ch0h=%d, ch1h=%d\n",ch0_l,ch1_l, ch0_l,ch0_h);
	ch1=(ch1_h<<8)|ch1_l;
	ch0=(ch0_h<<8)|ch0_l;
	adcval = (float)ch1/(float)ch0;	
	printf("ch0=%d,ch1=%d,adcval=%f\n",ch0,ch1,adcval);
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

/************************************************
* test luminosity 
* To check if luminosity is within certain range
*************************************************/

uint8_t test_luminosity(int32_t fd)
{
	float lux_output;
	lux_output = get_luminosity(fd);
	if(lux_output<-100 && lux_output>1800)
	{
		return error;
	}
	return success;
}
	

int main()
{
	float lux_output;
	uint8_t op,op1,op2,op3,op4,op5;
	int32_t error=0;	
	int32_t fd;
	uint8_t powerval=0,sensor_id=0, timer=0, interr=0;
	//i2c_init
	fd=open("/dev/i2c-2", O_RDWR);
	ioctl(fd, I2C_SLAVE, LUX_SLAVE_ADDR);	
	//power on	
	i2c_write(fd,POWER_ADDR);
	i2c_write(fd,POWER_ON_CMD);
	error=i2c_read(fd,&powerval,1);
	if(powerval==POWER_ON_CMD)
	{
		printf("the value of power is %x\n", powerval);
	}
	//test luminosity
	op1=test_luminosity(fd);
	if(op1==1)
	{
		printf("test luminosity successful\n");
	}
	//test control reg
	op2=control_reg_test(fd);
	if(op2==1)
	{
		printf("test control reg successful\n");
	}
	//test identification reg
	op3=id_reg_test(fd);
	if(op3==1)
	{
		printf("test identificarion register successful\n");
	}
	//test timing reg
	op4=timing_reg_test(fd);
	if(op4==1)
	{
		printf("test timing register successful\n");
	}
	//test interrupt threshold reg
	op5=int_threshold_test(fd);
	if(op5==1)
	{
		printf("test threshold successful\n");
	}
	//test interrupt control reg
	op=int_control_reg_test(fd);
	if(op==1)
	{
		printf("test interrupt control register successful\n");
	}
}
