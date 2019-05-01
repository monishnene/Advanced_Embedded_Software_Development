/******************************************
* client.c
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

/*****************
* Includes
*******************/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>

//ip addr
#define IP_ADDR "10.0.0.152"

int main(void)
{
	int sockfd, operation, send_data, received, data, acc_conn;
	struct sockaddr_in server_addr;
	struct hostent* hostptr;
	//socket create
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		perror("socket creation failed\n");
	}
	puts("socket creation successfull\n");
	memset((char*)&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port = htons(10001);
	hostptr=gethostbyname(IP_ADDR);
	memcpy(&server_addr.sin_addr,hostptr->h_addr,hostptr->h_length);
	acc_conn = connect(sockfd,(struct  sockaddr*)&server_addr, sizeof(server_addr)); //accept conn
	if((acc_conn)<0)
	{
		printf("server is not connection ready\n");
		exit(-1);
	}
	while(1)
	{
		received = read(sockfd,&data,sizeof(data));
		printf("%d received data from server\n", received);
		if(received==sizeof(data))
		{
			printf("The received temperature data is %d C\n", data);
		}
		else
		{
			printf("data read from the server is faulty\n");
		}
		
	  }
	  return 0;
}
