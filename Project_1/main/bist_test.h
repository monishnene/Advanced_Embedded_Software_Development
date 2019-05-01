/******************************************
* bist_test.h
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

#ifndef BIST_H
#define BIST_H

/*******************************************
* Includes
*******************************************/

#include "common.h"
#include <math.h>

/*******************************************
* Macros
*******************************************/

typedef enum //error or success enum
{
	error=0,
	success=1
}error_check;

uint8_t i2c_file(int32_t fd);
static uint8_t i2c_writeb(int32_t fd,uint8_t regval);
static uint8_t i2c_readb(int32_t fd,uint8_t* buffer,uint32_t size);
uint8_t cmdreg_write_test(int32_t fd);
uint8_t id_reg_test(int32_t fd);
uint8_t timing_reg_test(int32_t fd);
uint8_t i2c_readword(int32_t fd,uint8_t* buff);
uint16_t register_read(int32_t fd, uint8_t regval);
int bist_check(void);

#endif
