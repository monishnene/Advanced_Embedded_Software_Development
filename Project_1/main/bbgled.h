/***************************************
* bbgled.h
* Author: Monish Nene and Sanika Dongre
* Date created: 03/25/19
***************************************/

#ifndef BBGLED_H
#define BBGLED_H
#include "common.h"
#define	LED_COUNT 4

static uint8_t path[]="/sys/class/gpio/gpio5x/value";
static uint8_t pathx=21;
static uint8_t led_init_cmd[]="./led_init.sh";

void led_toggle(uint8_t led);
void led_off(uint8_t led);
void led_on(uint8_t led);
void led_init(void);

#endif
