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

#define slave_addr     (0x48)
#define tempregaddr    (00)
#define inicond        (0x60A0)
#define configregaddr  (0x01)
#define tlowregaddr    (0x02)
#define thighregaddr   (0x03)
#define highmask       (0x00FF)
#define interruptval   (0x62a0)
#define shutdown       (0x61a0)

typedef enum //error or success enum
{
	error=0,
	success=1
}error_check;

uint8_t i2c_file(int32_t fd);
uint8_t i2c_write(int32_t fd,uint8_t regval);
uint8_t i2c_read(int32_t fd,uint8_t* buffer,uint32_t size);
uint8_t cmdreg_write_test(int32_t fd);
uint8_t id_reg_test(int32_t fd);
uint8_t timing_reg_test(int32_t fd);
uint8_t i2c_readword(int32_t fd,uint8_t* buff);
uint16_t register_read(int32_t fd, uint8_t regval);
int bist_check(void);

#endif
