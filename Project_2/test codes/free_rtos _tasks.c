//****************************************************************************
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/uart.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "sensorlib/i2cm_drv.h"
#include "driverlib/i2c.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
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
#include "timers.h"
#include "semphr.h"
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"
#include "queue.h"
#include "semphr.h"
#include "drivers/pinout.h"
#include "drivers/buttons.h"

#define MQ_SIZE         (10)            // Size of the message queue
#define TASKSTACKSIZE   1024
#define STR_SIZE 200
#define LOGGER_PERIOD 20
#define THRESHOLD_PERIOD 1000
#define LED_PERIOD  1000
#define SENSOR_PERIOD 12e7
#define GAS_PERIOD 12e7
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

QueueHandle_t log_queue;
SemaphoreHandle_t sem_log,sem_read;
static volatile uint32_t log_counter=0;
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
TaskHandle_t thresholdfxnhandle, gasfxnhandle,loggerfxnhandle,uartfxnhandle,sensorfxnhandle;
uint8_t* error_msg[]={"The Log Queue is full, Data Lost","Log type not found"};
volatile unsigned long g_vulRunTimeStatsCountValue;
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
    // Enable the GPIO Peripheral used by the UART.
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Enable UART0
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Configure GPIO Pins for UART mode.
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);
}


void queue_adder(queue_data_t* data_send)
{
    data_send->time_now =  xTaskGetTickCount();
    if(log_counter==0)
    {
        xSemaphoreGive(sem_read);
    }
    if(log_counter < QUEUE_SIZE-1)
    {
        LEDWrite(0x0F, 0x00);
        //xQueueSend(log_queue,( void * )data_send,(TickType_t)10);
        log_counter++;
    }
    else if(log_counter == QUEUE_SIZE-1)
    {
        data_send->data=0;
        data_send->log_id=LOG_ERROR;
        LEDWrite(0x0F, 0x00);
        //xQueueSend(log_queue,( void * )data_send,(TickType_t)10);
        log_counter++;
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
        UARTprintf("Entering UART task\n");
       //command=UARTCharGet(0);
        //UARTgets(&received_data, sizeof(received_data));
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
    uint8_t time_str[30];
    uint8_t timeslice;
    queue_data_t received_data;
    while (condition)
    {
        UARTprintf("Entering logger task\n");
         //message queue receive
         xSemaphoreTake(sem_log, portMAX_DELAY);
         xSemaphoreTake(sem_read, portMAX_DELAY);
         while(log_counter<1);
         if(log_counter>0)
         {

             printf("hello this is sanika\n");
             /*if(xQueueReceive(log_queue, &(received_data), portMAX_DELAY)==0)
             {
                 continue;
             }*/
         }
         log_counter--;
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
         if(log_counter)
         {
             xSemaphoreGive(sem_log);
             xSemaphoreGive(sem_read);
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

void i2c_init(void)
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

void gasFxn(void)
{
    queue_data_t data_send;
    while (condition)
    {
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
        UARTprintf("Entering Threshold task\n");
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

void sensorFxn(void)
{
    uint16_t data_op[2];
    queue_data_t data_send;
    while (condition)
    {
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


// Global variable to hold the system clock speed.
uint32_t g_ui32SysClock;


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif



//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}


/**
 * brief Function to create and enable timers for Temperature Sensors and LED Toggling.
 * TIMER0 corresponds to Temperature timer.
 * TIMER1 corresponds to LED Timer
 * @return void
 */
void timer_init()
{

    // Enable the peripherals for Timers.
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    //ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);

    // Enable processor interrupts.
    //ROM_IntMasterEnable();

    // Configure the 32-bit periodic timer.
    //ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    //ROM_TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

   // ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, SENSOR_PERIOD);
    //ROM_TimerLoadSet(TIMER1_BASE, TIMER_A, GAS_PERIOD);

    // Setup the interrupts for the timer timeout.
    //ROM_IntEnable(INT_TIMER0A);
    //ROM_IntEnable(INT_TIMER1A);
    //ROM_TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    //ROM_TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

    // Enable the timers.
    //ROM_TimerEnable(TIMER0_BASE, TIMER_A);
    //ROM_TimerEnable(TIMER1_BASE, TIMER_A);
}


/**
 * @brief  Initialize FreeRTOS and start the initial set of tasks.
 * @return int
 */
int main(void)
{
    // Configure the system frequency.
    g_ui32SysClock = MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 12e7);

    // Initialize the UART and configure it for 115,200, 8-N-1 operation.
    uart_init();

    //Configuring I2C
    i2c_init();

    //Creating Semaphores
    condition = 1;
    msg = (uint8_t*)malloc(STR_SIZE);
    //msg[200];
    sem_log = xSemaphoreCreateBinary();
    sem_read = xSemaphoreCreateBinary();
   // log_queue = xQueueCreate(QUEUE_SIZE, sizeof(queue_data_t));
    xTaskCreate(thresholdFxn, (const portCHAR *)"thresholdstart",
                   configMINIMAL_STACK_SIZE, NULL, 1, &thresholdfxnhandle);

    xTaskCreate(loggerFxn, (const portCHAR *)"loggerstart",
                   configMINIMAL_STACK_SIZE, NULL, 1, &loggerfxnhandle);

    xTaskCreate(uartFxn, (const portCHAR *)"uartcomm",
                   configMINIMAL_STACK_SIZE, NULL, 1, &uartfxnhandle);

    //Creates and Enables Timers
    timer_init();

    // Start the scheduler.  This should not return.
    vTaskStartScheduler();

    // In case the scheduler returns for some reason, print an error and loop forever.
    while(condition)
    {
        // Do Nothing.
    }
}
