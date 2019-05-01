/******************************************
* server.c
* Remote socket task
* Author: Sanika Dongre and Monish Nene
* Date created: 03/29/19
*******************************************/

/*******************************************
* Includes
*******************************************/

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

int main(void)
{
	int sockfd, conn, fork_child=1, serverlen, data=10, received, input, send_data, temp=20;
	struct hostent* hostptr;
	struct sockaddr_in server_addr, client_addr;
	//socket create
	sockfd = socket(AF_INET, SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket creation error\n");
	}
	puts("socket created successful\n");
	memset((char*)&server_addr,0,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_port= htons(10001);
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serverlen = sizeof(struct sockaddr_in);
	//bind
	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
	{
		perror("socket binding error\n");
	}
	puts("socket binding successful\n");
	//listen
	if(listen(sockfd,5)<0)
	{
		perror("socket listening error\n");
	}
	puts("socket listening successful\n");
	while(1)
	{
		//connection accept
		conn=accept(sockfd,(struct sockaddr*)&client_addr,&serverlen);
		if(conn<0)
		{
			perror("connection accept failed\n");
		}
		puts("connection accepted\n");
		fork_child=fork(); //fork
		if(fork_child==0)
		{
			send_data=send(conn,(void*)&temp,sizeof(temp),0); //send temp data to client
			printf("%d data sent to client\n", send_data);
		}
	}
	//close socket
	close(sockfd);
	printf("\n socket closed from server side\n");
	return 0;
}
