#ifndef BBGLED_H
#define BBGLED_H 
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <linux/gpio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#define STR_SIZE 30
#define	LED_COUNT 4

static uint8_t path[]="/sys/class/gpio/gpio5x/value";
static uint8_t pathx=21;
static uint8_t led_init_cmd[]="./led_init.sh";

void led_toggle(uint8_t led);
void led_off(uint8_t led);
void led_on(uint8_t led);
void led_init(void);

#endif
