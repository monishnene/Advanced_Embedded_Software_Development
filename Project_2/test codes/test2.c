#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

//reference http://tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B9600           
#define UART0 "/dev/ttyO0"
#define UART4 "/dev/ttyO4"	 
#define _POSIX_SOURCE 1 /* POSIX compliant source */

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

void main(void)
{
  int fd_uart0,fd_uart4,c, res;
  struct termios oldtio0,oldtio4,newtio0,newtio4;
  char buffer_in[255];
  char buffer_out[]="\nHello World!\nUart is working ;-)\n";
/* 
  Open modem device for reading and writing and not as controlling tty
  because we don't want to get killed if linenoise sends CTRL-C.
*/
 fd_uart4 = open(UART4, O_RDWR | O_NOCTTY ); 
 if (fd_uart4 <0)
{
printf("uart4 not found\n");
}
else
{
printf("uart4 found\n");
}
 tcgetattr(fd_uart4,&oldtio4); /* save current serial port settings */

/* 
  BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
  CRTSCTS : output hardware flow control (only used if the cable has
            all necessary lines. See sect. 7 of Serial-HOWTO)
  CS8     : 8n1 (8bit,no parity,1 stopbit)
  CLOCAL  : local connection, no modem contol
  CREAD   : enable receiving characters
*/
 newtio4.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
/*
  IGNPAR  : ignore bytes with parity errors
  ICRNL   : map CR to NL (otherwise a CR input on the other computer
            will not terminate input)
  otherwise make device raw (no other input processing)
*/
 newtio4.c_iflag = IGNPAR | ICRNL;
 
/*
 Raw output.
*/
 newtio4.c_oflag = 0;
 
/*
  ICANON  : enable canonical input
  disable all echo functionality, and don't send signals to calling program
*/
 newtio4.c_lflag = ICANON;
 
/* 
  initialize all control characters 
  default values can be found in /usr/include/termios.h, and are given
  in the comments, but we don't need them here
*/
 newtio4.c_cc[VINTR]    = 0;     /* Ctrl-c */ 
 newtio4.c_cc[VQUIT]    = 0;     /* Ctrl-\ */
 newtio4.c_cc[VERASE]   = 0;     /* del */
 newtio4.c_cc[VKILL]    = 0;     /* @ */
 newtio4.c_cc[VEOF]     = 4;     /* Ctrl-d */
 newtio4.c_cc[VTIME]    = 0;     /* inter-character timer unused */
 newtio4.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
 newtio4.c_cc[VSWTC]    = 0;     /* '\0' */
 newtio4.c_cc[VSTART]   = 0;     /* Ctrl-q */ 
 newtio4.c_cc[VSTOP]    = 0;     /* Ctrl-s */
 newtio4.c_cc[VSUSP]    = 0;     /* Ctrl-z */
 newtio4.c_cc[VEOL]     = 0;     /* '\0' */
 newtio4.c_cc[VREPRINT] = 0;     /* Ctrl-r */
 newtio4.c_cc[VDISCARD] = 0;     /* Ctrl-u */
 newtio4.c_cc[VWERASE]  = 0;     /* Ctrl-w */
 newtio4.c_cc[VLNEXT]   = 0;     /* Ctrl-v */
 newtio4.c_cc[VEOL2]    = 0;     /* '\0' */

/* 
  now clean the modem line and activate the settings for the port
*/
 //tcflush(fd_uart0, TCIFLUSH);
 //tcsetattr(fd_uart0,TCSANOW,&newtio0);
 tcflush(fd_uart4, TCIFLUSH);
 tcsetattr(fd_uart4,TCSANOW,&newtio4);

/*
  terminal settings done, now handle input
  In this example, inputting a 'z' at the beginning of a line will 
  exit the program.
*/
 //while (STOP==FALSE) {     /* loop until we have a terminating condition */
 /* read blocks program execution until a line terminating character is 
    input, even if more than 255 chars are input. If the number
    of characters read is smaller than the number of chars available,
    subsequent reads will return the remaining chars. res will be set
    to the actual number of characters actually read */
    //res = write(fd_uart,buffer_out,strlen(buffer_out));
    res = read(fd_uart4,buffer_in,255); 
    buffer_out[res]=0;             /* set end of string, so we can printf */
    printf(":%s:%d\n", buffer_in, res);
    //if (buf[0]=='z') STOP=TRUE;
 //}
 /* restore the old port settings */
}

