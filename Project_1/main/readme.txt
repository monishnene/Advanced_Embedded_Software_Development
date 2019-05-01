Functions common for light and temperature sensor:
i2c_write():  It will take the  file descriptor and the address of the register as the parameter and write to the register of the temperature and light sensor respectively.
•	i2c_read(): It will take the file descriptor and the address of the register as a parameter and read the respective sensor.
•	i2c_file():  to open the file in (/dev/i2c-2) folder and to perform ioctl taking into account the respective sensor’s slave address.
Temperature task:
•	In temperature_run(): run temperature measurement function
•	get_temperature(): Reads data from i2c temperature and returns output in Celsius
•	temperature init: to initialize the temp measurement
•	temperature_read(): Read data from temperature sensor and log it.
•	In temptest.c for unit test:
register_read(): used for reading a register. This API is used for reading the register 
register_write(): used for writing a register 
•	get_temperature(): get the latest temperature value and convert into required temperature units.

Light task:
•	light_init(): To initialize semaphores and shared memory
•	get_luminosity(): get the latest luminosity/lux value 
•	day_night() to check if it is day time or night time
•	light_read(): This function is used to read luminosity from sensor and log it
Error handling  (done via the main task):
•	led_on(): To turn on the leds
•	led_off(): To turn off the leds
•	led_toggle(): to toggle the leds
•	main(): To toggle the leds in a 

Remote request task:
•	server.c : handle request from client
Log synchronization task:
•	logger_init(): initialize resources required for logger
•	logger(): o continuously run and log data
•	log_creator(): To log the data to the log file. It takes the Log level : error, info as the first parameter and the string to be printed as the second parameter.
Main task:
•	temperature_run():To check if the tasks/processes are alive and return error if any process/thread has stopped.
•	Light_run(): to run light measurement function
•	Logger run(): run logger functions
•	Server_run(): run server functions
•	Logfile_Setup(): backup of old file and create a new one
•	Temperature_thread(): create and join temp thread
•	Light_thread(): create and join light thread
•	Logger_thread(): create and join logger thread
•	Server_thread(): create and join server thread
•	System_end(): exit smoothly when signint is sent to main process
•	Heartbeat(): check all threads running properly and alive
•	Join threads(): to join threads
•	Timer_init(): to initialize the timer
•	Kill_server(): to kill thread with usr2
•	Kill_logger(): to kill thread with usr1
•	System_init(): to initialize all resources of the project
