/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//I have refferred the TI resource explorer functions and https://www.digikey.com/eewiki/display/microcontroller/I2C+Communication+with+the+TI+Tiva+TM4C123GXL for I2C communication

/*
 *  ======== empty.c ========
 */
/* Board Header file */
#include "Board.h"


/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <xdc/runtime/Error.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdbool.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include <ti/sysbios/knl/semaphore.h>
#include <ti/sysbios/knl/Clock.h>
#include "FreeRTOS.h"
#include "queue.h"


/*****************************
* Global variables
* shared mem
*****************************
*****************************/
#define TASKSTACKSIZE   1024
#define STR_SIZE 200
#define LOGGER_PERIOD 20
#define LED_PERIOD  100
#define TEMPERATURE_PERIOD 1000
#define TEMP_SLAVE_ADDR (0x48)
#define TEMP_REG_ADDR   (0x00)

//semaphore
Semaphore_Struct sem_write_struct,sem_read_struct;
Semaphore_Handle sem_write,sem_read;
Semaphore_Params sem_write_params,sem_read_params;
uint8_t log_type;
Task_Struct led_struct;
Task_Struct temperature_struct;
Task_Struct logger_struct;
Char led_stack[TASKSTACKSIZE];
Char logger_stack[TASKSTACKSIZE];
Char temperature_stack[TASKSTACKSIZE];
uint8_t* logfile;
int8_t flag;
uint8_t* msg;
int32_t shm_light,shm_temp,n,i,condition;
struct tm *time_and_date;
static uint8_t* logtype[]={"LOG_LED","LOG_TEMPERATURE"};
UART_Handle uart;
UART_Params uartParams;
QueueHandle_t temp_queue,led_queue;

typedef enum
{
    LOG_LED=0,
    LOG_TEMPERATURE=1,
}logtype_t;

typedef struct
{
    int32_t temperature;
    int32_t time_now;
}temp_data_t;

typedef struct
{
    int32_t time_now;
    int32_t toggle_count;
}led_data_t;

exit_handler()
{
    condition=0;
}

uart_init()
{
   /* Create a UART with data processing off. */
   UART_Params_init(&uartParams);
   uartParams.writeDataMode = UART_DATA_BINARY;
   uartParams.readDataMode = UART_DATA_BINARY;
   uartParams.readReturnMode = UART_RETURN_FULL;
   uartParams.readEcho = UART_ECHO_OFF;
   uartParams.baudRate = 9600;
   uart = UART_open(Board_UART0, &uartParams);
   if (uart == NULL) {
       System_abort("Error opening the UART");
   }
}

/*
 *  ======== heartBeatFxn ===='====
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
Void ledtoggleFxn(UArg arg0, UArg arg1)
{
    static int32_t count=0;
    led_data_t data;
    uint32_t time_now =  Clock_getTicks();
    while (condition)
    {
        Task_sleep((unsigned int)arg0);
        GPIO_toggle(Board_LED1);
        GPIO_toggle(Board_LED0);
        Semaphore_pend(sem_write, BIOS_WAIT_FOREVER);
        data.time_now =  Clock_getTicks();
        data.toggle_count=++count;
        xQueueSend(led_queue,( void * )&data,( TickType_t ) 10 );
        flag=LOG_LED;
        Semaphore_post(sem_read);
    }
}

Void loggerFxn(UArg arg0, UArg arg1)
{
    temp_data_t data2;
    led_data_t data1;
    while (condition)
    {
        Task_sleep((unsigned int)arg0);

        //message queue receive
        Semaphore_pend(sem_read, BIOS_WAIT_FOREVER);
        if(flag==LOG_LED)
        {
            xQueueReceive(led_queue, &(data1), ( TickType_t ) 10 );
            sprintf(msg,"time:%dmsec\tMonish Nene\tLOG_LED\tLED TOGGLE COUNT = %d\n\r",data1.time_now,data1.toggle_count);
        }
        else if(flag==LOG_TEMPERATURE)
        {
            xQueueReceive(temp_queue, &(data2), ( TickType_t ) 10 );
            sprintf(msg,"time:%dmsec\tMonish Nene\tLOG_TEMPERATURE\tTemperature: %dC, %dF, %dK\n\r",data2.time_now,data2.temperature,((data2.temperature*9)/5)+32,data2.temperature+273);
        }
        UART_write(uart,msg,strlen(msg));
        Semaphore_post(sem_write);
    }
    free(msg);
}

void InitI2C0(void)
{
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    //clear I2C FIFOs
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}

Void temperatureFxn(UArg arg0, UArg arg1)
{
    temp_data_t data;
    int8_t rx_MSB=0,rx_LSB=0;
    while (condition)
    {
        Task_sleep((unsigned int)arg0);
        I2CMasterSlaveAddrSet(I2C0_BASE, TEMP_SLAVE_ADDR, false);

        //specify register to be read
        I2CMasterDataPut(I2C0_BASE, TEMP_REG_ADDR);

        //send control byte and register address byte to slave device
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        //wait for MCU to finish transaction
        while(I2CMasterBusy(I2C0_BASE));

        //specify that we are going to read from slave device
        I2CMasterSlaveAddrSet(I2C0_BASE, TEMP_SLAVE_ADDR, true);

        //send control byte and read from the register we
        //specified
        I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

        //wait for MCU to finish transaction
        while(I2CMasterBusy(I2C0_BASE));

        //return data pulled from the specified register
        rx_MSB=I2CMasterDataGet(I2C0_BASE);

        //send control byte and read from the register we
       //specified
       I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);
       //wait for MCU to finish transaction
       while(I2CMasterBusy(I2C0_BASE));
       rx_LSB=I2CMasterDataGet(I2C0_BASE);

       data.temperature = (((rx_MSB << 8) | rx_LSB) >> 4)/16.0;
       data.time_now =  Clock_getTicks();
       Semaphore_pend(sem_write, BIOS_WAIT_FOREVER);
       flag=LOG_TEMPERATURE;
       xQueueSend(temp_queue,( void * )&data,( TickType_t ) 10 );
       //insert message queue
       Semaphore_post(sem_read);
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params led_task,logger_task,temperature_task;
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initI2C();
    Board_initUART();
    InitI2C0();
    condition = 1;
    temp_queue = xQueueCreate( 10, sizeof(temp_data_t) );
    led_queue = xQueueCreate( 10, sizeof(led_data_t) );
    msg = (uint8_t*)malloc(STR_SIZE);

    //semaphore
    Semaphore_Params_init(&sem_write_params);
    Semaphore_construct(&sem_write_struct, 1, &sem_write_params);
    sem_write = Semaphore_handle(&sem_write_struct);
    Semaphore_Params_init(&sem_read_params);
    Semaphore_construct(&sem_read_struct, 0, &sem_read_params);
    sem_read = Semaphore_handle(&sem_read_struct);

    /* Construct led Task  thread */
    Task_Params_init(&led_task);
    led_task.arg0 = LED_PERIOD;
    led_task.stackSize = TASKSTACKSIZE;
    led_task.stack = &led_stack;
    Task_construct(&led_struct, (Task_FuncPtr)ledtoggleFxn, &led_task, NULL);

    /* Construct logger Task  thread */
    Task_Params_init(&logger_task);
    logger_task.arg0 = LOGGER_PERIOD;
    logger_task.stackSize = TASKSTACKSIZE;
    logger_task.stack = &logger_stack;
    Task_construct(&logger_struct, (Task_FuncPtr)loggerFxn, &logger_task, NULL);

    /* Construct temperature  thread */
    Task_Params_init(&temperature_task);
    temperature_task.arg0 = TEMPERATURE_PERIOD;
    temperature_task.stackSize = TASKSTACKSIZE;
    temperature_task.stack = &temperature_stack;
    Task_construct(&temperature_struct, (Task_FuncPtr)temperatureFxn, &temperature_task, NULL);

     /* Turn on user LED */

    System_printf("Starting the example\nSystem provider is set to SysMin. "
                  "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();
    uart_init();
    /* Start BIOS */
    BIOS_start();
    return (0);
}
