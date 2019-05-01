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
#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "drivers/pinout.h"
#include "utils/uartstdio.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "math.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include "drivers/pinout.h"
#include "driverlib/gpio.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
#include <stdlib.h>
// TivaWare includes
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/i2c.h"

#include <stdbool.h>
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "driverlib/pin_map.h"
// TivaWare includes
#include "inc/hw_types.h"
#include "driverlib/rom.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "queue.h"
#include "main.h"
#include "timers.h"
#include "semphr.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
/* Global variables
* shared mem
*****************************
*****************************/
#define TASKSTACKSIZE   1024
#define STR_SIZE 200
#define LOGGER_PERIOD 20
#define THRESHOLD_PERIOD 1000
#define LED_PERIOD  1000
#define SENSOR_PERIOD 1000
#define GAS_PERIOD 1000
#define HUMIDITY_PERIOD 1000
#define TEMP_SLAVE_ADDR (0x48)
#define TEMP_REG_ADDR   (0x00)
#define UART_PERIOD 1
#define QUEUE_SIZE 100
#define AUTOMATIC_MODE 1
#define MANUAL_MODE 0
#define SI7021_SLAVE_ADDRESS (0x40)
#define TEMP_ADDRESS (0xE3)
#define HUMIDITY_ADDRESS (0xE5)

//semaphore
//Semaphore_Struct sem_log_struct,sem_read_struct;
//Semaphore_Params sem_log_params,sem_read_params;
SemaphoreHandle_t sem_read, sem_log;
uint8_t log_type;
char logger_stack[TASKSTACKSIZE];
char sensor_stack[TASKSTACKSIZE];
char uart_stack[TASKSTACKSIZE];
char gas_stack[TASKSTACKSIZE];
char threshold_stack[TASKSTACKSIZE];
uint8_t* logfile;
uint8_t* msg;
uint8_t fans_on=0;
bool buzzer=0;
bool remote_mode = AUTOMATIC_MODE;
int32_t current_temperature,current_gas,condition;
double current_humidity;
bool fans[5]={0,0,0,0,0};
int32_t temperature_threshold[5]={20,25,30,35,50};
int32_t humidity_threshold[5]={20,40,60,80,90};
int32_t gas_threshold[5]={20,40,60,80,90};
QueueHandle_t log_queue;
TaskHandle_t thresholdfxnhandle, gasfxnhandle,loggerfxnhandle,uartfxnhandle,sensorfxnhandle;
uint8_t* error_msg[]={"The Log Queue is full, Data Lost","Log type not found"};

typedef enum
{
    QUEUE_FULL=0,
    ERROR_LOGTYPE=1,
}error_t;

typedef enum
{
    LOG_LED=0,
    LOG_TEMPERATURE=1,
    LOG_HUMIDITY=2,
    LOG_GAS=3,
    LOG_THRESHOLD=4,
    LOG_COMMAND=5,
    LOG_FAN=6,
    LOG_ERROR=7,
}logtype_t;

typedef struct
{
    logtype_t log_id;
    int32_t data;
    int32_t time_now;
}queue_data_t;

typedef enum
{
    LOG_DATA='A',
    GET_TEMPERATURE='B',
    GET_HUMIDITY='C',
    GET_GAS='D',
    GET_THRESHOLD='E',
    GET_FAN='F',
    CHANGE_MODE='G',
    CHANGE_TEMPERATURE_THRESHOLD='H',
    CHANGE_HUMIDITY_THRESHOLD='I',
    CHANGE_GAS_THRESHOLD='J',
    BUZZER_ON='K',
    BUZZER_OFF='L',
    FORCE_CHANGE_FANS='M',
    GET_BUZZER='N',
}uart_command_t;

typedef struct
{
    uart_command_t command_id;
    int32_t data;
    int32_t time_now;
}uart_data_t;

void exit_handler(void)
{
    condition=0;
}

void uart_init(void)
{
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
        ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
        ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
        ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
        UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
        UARTStdioConfig(0, 115200, 16000000);

}

void UART_send(char* ptr, int length)
{

               while(length != 0)
               {
                   UARTCharPut(UART0_BASE, *ptr);
                   ptr++;
                   length--;
               }
 }

void queue_adder(queue_data_t* data_send)
{
    data_send->time_now =  xTaskGetTickCount();
    if(uxSemaphoreGetCount(sem_read) < QUEUE_SIZE-1)
    {
        LEDWrite(0x0F, 0x00);
        xQueueSend(log_queue,( void * )data_send,( TickType_t ) 10 );
        xSemaphoreGive(sem_read);
    }
    else if(uxSemaphoreGetCount(sem_read) == QUEUE_SIZE-1)
    {
        data_send->data=0;
        data_send->log_id=LOG_ERROR;
        LEDWrite(0x0F, 0x00);
        xQueueSend(log_queue,( void * )data_send,( TickType_t ) 10 );
        xSemaphoreGive(sem_read);
    }
}

void buzzer_control(void)
{
    if(buzzer)
    {
        //buzzer on
    }
    else
    {
        //buzzer off
    }
}

void Fan_update(int8_t value)
{
    queue_data_t data_send;
    uint8_t i=0;
    for(i=0;i<5;i++)
    {
        fans[i]=i<=value?1:0;
    }
    if(fans[4])
    {
        buzzer=1;
        buzzer_control();
    }
    else
    {
        buzzer=0;
        buzzer_control();
    }
    data_send.data=value;
    data_send.log_id=LOG_FAN;
    queue_adder(&data_send);
}

void uartFxn(void* ptr)
{
    uint8_t command;
    uart_data_t received_data,send_data;
    queue_data_t data_log;
    while(condition)
    {
        //UART_read(uart, &command, 1);
        //UART_read(uart, &recevied_data, sizeof(uart_data_t));
        send_data.command_id=received_data.command_id;
        send_data.time_now=xTaskGetTickCount();
        data_log.log_id=LOG_COMMAND;
        data_log.data=received_data.command_id;
        queue_adder(&data_log);
        switch(command)
        {
            case LOG_DATA:
            {
                xSemaphoreGive(sem_log);
                break;
            }

            case GET_TEMPERATURE:
            {
                send_data.data=current_temperature;
                UARTprintf("The temperature is %d",send_data.data);
                break;
            }

            case GET_HUMIDITY:
            {
                send_data.data=current_humidity;
                UARTprintf("The humidity is %d",send_data.data);
                break;
            }

            case GET_GAS:
            {
                send_data.data=current_gas;
                UARTprintf("The gas data is %d", send_data.data);
                break;
            }

            case GET_THRESHOLD:
            {
                UARTprintf("The temperature threshold is %d", temperature_threshold);
                UARTprintf("The humidity threshold is %d", humidity_threshold);
                UARTprintf("The gas threshold is %d", gas_threshold);
                break;
            }

            case GET_FAN:
            {
                send_data.data=fans_on;
                UARTprintf("The fans on is %d", send_data.data);
                break;
            }

            case GET_BUZZER:
            {
                send_data.data=buzzer;
                UARTprintf("The buzzer is %d", send_data.data);
                break;
            }

            case CHANGE_MODE:
            {
                if(received_data.data<2)
                {
                    remote_mode=received_data.data;
                }
                break;
            }

            case CHANGE_TEMPERATURE_THRESHOLD:
            {
                //UART_read(uart, &temperature_threshold, sizeof(temperature_threshold));
                break;
            }

            case CHANGE_HUMIDITY_THRESHOLD:
            {
                //UARTprintf(uart, &humidity_threshold, sizeof(temperature_threshold));
                break;
            }

            case CHANGE_GAS_THRESHOLD:
            {
                //UARTprintf(uart, &gas_threshold, sizeof(temperature_threshold));
                break;
            }

            case BUZZER_ON:
            {
                remote_mode=MANUAL_MODE;
                buzzer=1;
                //buzzer_control();
                break;
            }

            case BUZZER_OFF:
            {
                remote_mode=MANUAL_MODE;
                buzzer=0;
                //buzzer_control();
                break;
            }

            case FORCE_CHANGE_FANS:
            {
                if(received_data.data<6)
                {
                    remote_mode=MANUAL_MODE;
                    fans_on=received_data.data;
                    Fan_update(fans_on);
                }
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

void loggerFxn(void* ptr)
{
    uint8_t* time_str = (uint8_t*)malloc(30);
    uint8_t timeslice;
    queue_data_t received_data;
    while (condition)
    {
         //message queue receive
         xSemaphoreTake(sem_log, portMAX_DELAY);
         xSemaphoreTake(sem_read, portMAX_DELAY);
         xQueueReceive(log_queue, &(received_data), ( TickType_t ) 10 );
         sprintf(time_str,"time: %d sec %d msec\t",received_data.time_now/1000,received_data.time_now%1000);
         strcpy(msg,time_str);
         timeslice=strlen(time_str);
         switch(received_data.log_id)
         {
            case LOG_LED:
            {
                sprintf(msg+timeslice,"LOG_LED\tled toggle count = %d\n\r",received_data.data);
                break;
            }

            case LOG_TEMPERATURE:
            {
                sprintf(msg+timeslice,"LOG_TEMPERATURE\tTemperature: %dC, %dF, %dK\n\r",received_data.data,((received_data.data*9)/5)+32,received_data.data+273);
                break;
            }

            case LOG_COMMAND:
            {
                sprintf(msg+timeslice,"LOG_COMMAND\tCommand received: %c\n\r",received_data.data);
                break;
            }

            case LOG_HUMIDITY:
            {
                sprintf(msg+timeslice,"LOG_HUMIDITY\tHumidity: %d%%\n\r",received_data.data);
                break;
            }

            case LOG_GAS:
            {
                sprintf(msg+timeslice,"LOG_GAS\t\tThe CO value is %d ppm\n\r",received_data.data);
                break;
            }

            case LOG_FAN:
            {
                sprintf(msg+timeslice,"LOG_FAN\tFans ON: %d\n\r",received_data.data);
                break;
            }

            case LOG_THRESHOLD:
            {
                sprintf(msg+timeslice,"LOG_THRESHOLD\tTHRESHOLD CHECKED\n\r");
                break;
            }

            case LOG_ERROR:
            {
                sprintf(msg+timeslice,"LOG_ERROR\t%s\n\r",error_msg[received_data.data]);
                break;
            }

            default:
            {
                sprintf(msg+timeslice,"LOG_ERROR\t%s data:%d\n\r",error_msg[ERROR_LOGTYPE],received_data.data);
                break;
            }
         }
         if(uxSemaphoreGetCount(sem_read))
         {
             xSemaphoreGive(sem_log);
         }
         else
         {
             UARTprintf("The message is %s", msg);
             sprintf(msg+timeslice,"LOG_END\n\r");
         }
         UARTprintf("The message is %s", msg);
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

    //Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    //Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    //Enable and initialize the I2C0 master module.  Use the system clock for
    //the I2C0 module.  The last parameter sets the I2C data transfer rate.
    //If false the data rate is set to 100kbps and if true the data rate will
    //be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    //clear I2C FIFOs
  //  HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}

void gasFxn(void* ptr)
{
    queue_data_t data_send;
    while (condition)
    {
       vTaskDelay(GAS_PERIOD);
       SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
       while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC1));
       GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_2);
       ADCSequenceConfigure(ADC1_BASE,3,ADC_TRIGGER_PROCESSOR,0);
       ADCSequenceStepConfigure(ADC1_BASE,3,0,ADC_CTL_CH1|ADC_CTL_IE|ADC_CTL_END);
       ADCSequenceEnable(ADC1_BASE,3);
       ADCProcessorTrigger(ADC1_BASE,3);
       while(!ADCIntStatus(ADC1_BASE,3,false));
       ADCIntClear(ADC1_BASE,3);
       uint32_t data_op[1];
       ADCSequenceDataGet(ADC1_BASE,3,data_op);
       current_gas = (((data_op[0]*3.3)/4095)*1.0698);
       current_gas = 3.027*(pow(2.718,current_gas));
       data_send.data = current_gas;
       data_send.log_id=LOG_GAS;
       queue_adder(&data_send);
    }
}

void thresholdFxn(void* ptr)
{
    uint8_t i=0;
    queue_data_t data_send;
    while (condition)
    {
        vTaskDelay(THRESHOLD_PERIOD);
        //
        for(i=0;i<5;i++)
        {
            if((current_temperature<temperature_threshold[i])&&(current_humidity<humidity_threshold[i])&&(current_gas<gas_threshold[i]))
            {
                break;
            }
        }
        if((i!=fans_on)&&remote_mode)
        {
            fans_on=i;
            Fan_update(fans_on);
        }
        data_send.data=rand()%5;
        data_send.log_id=LOG_THRESHOLD;
        queue_adder(&data_send);
    }
}

void sensorFxn(void* ptr)
{
    uint16_t data_op[2];
    queue_data_t data_send;
    while (condition)
    {
       vTaskDelay(SENSOR_PERIOD);
       I2CMasterSlaveAddrSet(I2C0_BASE, SI7021_SLAVE_ADDRESS, false);
       I2CMasterDataPut(I2C0_BASE,TEMP_ADDRESS);
       I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_SEND);
       while(I2CMasterBusy(I2C0_BASE));
       I2CMasterSlaveAddrSet(I2C0_BASE,SI7021_SLAVE_ADDRESS,true);
       I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
       while(I2CMasterBusy(I2C0_BASE));
       data_op[0]=I2CMasterDataGet(I2C0_BASE);
       I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
       while(I2CMasterBusy(I2C0_BASE));
       data_op[1]=I2CMasterDataGet(I2C0_BASE);
       current_temperature=((((data_op[0]<<8|data_op[1])*175.72)/65536)-46.85);//replace this
       data_send.data=current_temperature;
       data_send.log_id=LOG_TEMPERATURE;
       queue_adder(&data_send);
       //humidity
       I2CMasterSlaveAddrSet(I2C0_BASE, SI7021_SLAVE_ADDRESS, false);
       I2CMasterDataPut(I2C0_BASE, HUMIDITY_ADDRESS);
       I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_SEND);
       while(I2CMasterBusy(I2C0_BASE));
       I2CMasterSlaveAddrSet(I2C0_BASE,SI7021_SLAVE_ADDRESS,true);
       I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
       while(I2CMasterBusy(I2C0_BASE));
       data_op[0]=I2CMasterDataGet(I2C0_BASE);
       I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE);
       while(I2CMasterBusy(I2C0_BASE));
       data_op[1]=I2CMasterDataGet(I2C0_BASE);
       current_humidity=(((data_op[0]<<8|data_op[1])*125)/65536)-6;
       data_send.data=current_humidity;
       data_send.log_id=LOG_HUMIDITY;
       queue_adder(&data_send);
    }
}

/*
 *  ======== main ========
 */
int main(void)
{
    uint32_t output_clock_rate_hz;
    output_clock_rate_hz = ROM_SysCtlClockFreqSet(
                               (SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN |
                                SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480),
                               SYSTEM_CLOCK);
    ASSERT(output_clock_rate_hz == SYSTEM_CLOCK);
   // Task_Params uart_task,logger_task,sensor_task,gas_task,threshold_task;
    InitI2C0();
    condition = 1;
    log_queue = xQueueCreate(QUEUE_SIZE, sizeof(queue_data_t));
    msg = (uint8_t*)malloc(STR_SIZE);
    sem_read= xSemaphoreCreateCounting(10,0);
    sem_log= xSemaphoreCreateBinary();
    //semaphore
    //Semaphore_Params_init(&sem_log_params);
   // Semaphore_construct(&sem_log_struct, 0, &sem_log_params);
  //  sem_log = Semaphore_handle(&sem_log_struct);
   // Semaphore_Params_init(&sem_read_params);
    //Semaphore_construct(&sem_read_struct, 0, &sem_read_params);
   //sem_read = Semaphore_handle(&sem_read_struct);

    xTaskCreate(sensorFxn, (const portCHAR *)"Sensorstart",
                    configMINIMAL_STACK_SIZE, NULL, 1, &sensorfxnhandle);

    xTaskCreate(thresholdFxn, (const portCHAR *)"thresholdstart",
                configMINIMAL_STACK_SIZE, NULL, 1, &thresholdfxnhandle);

    xTaskCreate(loggerFxn, (const portCHAR *)"loggerstart",
                configMINIMAL_STACK_SIZE, NULL, 1, &loggerfxnhandle);

    xTaskCreate(gasFxn, (const portCHAR *)"gassensor",
                configMINIMAL_STACK_SIZE, NULL, 1, &gasfxnhandle);

    xTaskCreate(uartFxn, (const portCHAR *)"uartcomm",
                configMINIMAL_STACK_SIZE, NULL, 1, &uartfxnhandle);

     /* Turn on user LED */
    /* SysMin will only print to the console when you call flush or exit */
    uart_init();
    /* Start BIOS */
    vTaskStartScheduler();
    return (0);
}
