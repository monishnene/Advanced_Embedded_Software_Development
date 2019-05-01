/******************************************
* temperature_read.h
* Author: Sanika Dongre and Monish Nene
* Date created: 03/26/19
*******************************************/

#ifndef TEMPERATURE_READ_H
#define TEMPERATURE_READ_H

/*******************************************
* Includes
*******************************************/

#include "common.h"
#include "bbgled.h"

/*******************************************
* Macros
*******************************************/

#define TEMP_SLAVE_ADDR	(0x48)
#define TEMP_REG_ADDR	(00)

void temperature_read(void);
void temperature_init(void);

#endif
