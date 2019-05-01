/***************************************
* bbgled.h
* Author: Monish Nene and Sanika Dongre
* Date created: 03/25/19
***************************************/

#ifndef BBGLED_H
#define BBGLED_H

#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define	LED_COUNT 4

static uint8_t path[]="/sys/class/gpio/gpio5x/value";
static uint8_t pathx=21;
static uint8_t led_init_cmd[]="./led_init.sh";

void led_toggle(uint8_t led);
void led_off(uint8_t led);
void led_on(uint8_t led);
void led_init(void);

#endif
