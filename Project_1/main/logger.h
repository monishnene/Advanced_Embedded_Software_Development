/******************************************
* logger.h
* Author: Monish Nene and Sanika Dongre
* Date created: 03/25/19
*******************************************/

#ifndef LOGGER_H
#define LOGGER_H

/*******************************************
* Includes
*******************************************/

#include "common.h"
#include "bbgled.h"

void logger_init(void);
void logger(void);
void log_creator(uint8_t logid, uint8_t* str);

#endif
