API Description:

Remote-Node:

Filename: main.c

Functions:

1)	void exit_handler(): To exit the handler
2)	void uart_send(uint8_t* ptr, uint32_t size):  To send the UART data
3)	void uart_receive(uint8_t* ptr, uint32_t size): To receive the UART data
4)	void queue_adder(queue_data_t* data_send): To add the data in the queue and send the message queue

5)	void buzzer_control(void): To turn the buzzer on or off

6)	void Fan_update(int8_t value): fans update based on the values – turn on or off

7)	void i2c_init(void): To initialize the I2C

8)	void gpio_init(void): To initialize the GPIO pins for the output devices

9)	void uart_init(void): To initialize the uart and configure it

10)	void UARTFxn(void* ptr): Task to send and receive data over UART from the remote node to the control node.

11)	void loggerFxn(void* ptr): Task to log the data

12)	void gasFxn(void* ptr): Task to send the gas data readings in ppm to the control node from the remote node
13)   void thresholdFxn(void* ptr): Task to turn on/off the fans and the buzzer based on the set threshold values
14)   void __error__(char* pcFilename, uint32_t ui32Line): Assert error() function FREERTOS


Control-Node:

Filename: control_node.c

functions:

fork() operation in main to separate the logger task and the GUI designed

1)   void* logger(void* ptr): Logger function to log the data
2)   void logfile_setup(void): To setup the logfile function
3)   int32_t timer_init(void): To initialize the timer – periodic timer for 10 seconds
4)   void system_end(int sig): To change the condition for system_end
5) void uart_init(void): uart initialization 
6)  void termios_init(void): To initialize the terminal for saving the current port settings and setting the conditions for uart using proper flags
. 
Project Plan:
