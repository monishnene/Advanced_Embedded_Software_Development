#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <sys/types.h> 
#include <linux/kernel.h>
#define STRING_SIZE 256
#define DATA_SIZE 256
#define True 1
#define False 0

void cronlog(int32_t count,int32_t* data)
{
	uint8_t* str= (uint8_t*) malloc(STRING_SIZE);
	int32_t i=0;
	pid_t process_id=getpid();
	uid_t user_id=getuid();
	time_t t = time(NULL);
	struct tm *time_and_date = localtime(&t);
	sprintf(str,"Time & Date : %sProcess Id = %d\nUser ID = %d\nSorted Data = [",asctime(time_and_date),process_id,user_id);
	FILE* fptr = fopen("cronlog.txt","a");
	fwrite(str, 1, strlen(str), fptr);
	for(i=0;i<count;i++)
	{
		sprintf(str," %d,",data[i]);
		fwrite(str, 1, strlen(str), fptr);
	}
	sprintf(str,"]\n\n");
	fwrite(str, 1, strlen(str), fptr);
	free(str);	
	fclose(fptr);
	return;
}

void main()
{
	int32_t i=0,error_check=0;
	int32_t data[DATA_SIZE],output[DATA_SIZE];
	srand(time(NULL));
	for(i=0;i<DATA_SIZE;i++)
	{
		data[i]=rand();
	}
	printf("\nCase 1: ");
	error_check=syscall(398,data,DATA_SIZE,output);
	if(error_check!=0)
	{
		if(error_check == 1002)
		{	
			printf("Error: NULL pointer passed as buffer\n");
		}
		else if(error_check == 1001)
		{	
			printf("Error: Negative size passed\n");
		}
		else if(error_check == 1000)
		{
			printf("Error: Data size is less than 256");
		}
		else
		{
			perror("\nError");
		}
	}
	else
	{
		cronlog(DATA_SIZE,output);	
	}
	printf("\nCase 2: ");
	error_check=syscall(398,NULL,DATA_SIZE,output);
	if(error_check!=0)
	{
		if(error_check == 1002)
		{	
			printf("Error: NULL pointer passed as buffer\n");
		}
		else if(error_check == 1001)
		{	
			printf("Error: Negative size passed\n");
		}
		else if(error_check == 1000)
		{
			printf("Error: Data size is less than 256");
		}
		else
		{
			perror("\nError");
		}
	}
	else
	{
		cronlog(DATA_SIZE,output);	
	}
	printf("\nCase 3: ");
	error_check=syscall(398,data,DATA_SIZE,NULL);
	if(error_check!=0)
	{
		if(error_check == 1002)
		{	
			printf("Error: NULL pointer passed as buffer\n");
		}
		else if(error_check == 1001)
		{	
			printf("Error: Negative size passed\n");
		}
		else if(error_check == 1000)
		{
			printf("Error: Data size is less than 256");
		}
		else
		{
			perror("\nError");
		}
	}
	else
	{
		cronlog(DATA_SIZE,output);	
	}
	printf("\nCase 4: ");
	error_check=syscall(398,data,-33,output);
	if(error_check!=0)
	{
		if(error_check == 1002)
		{	
			printf("Error: NULL pointer passed as buffer\n");
		}
		else if(error_check == 1001)
		{	
			printf("Error: Negative size passed\n");
		}
		else if(error_check == 1000)
		{
			printf("Error: Data size is less than 256");
		}
		else
		{
			perror("\nError");
		}
	}
	else
	{
		cronlog(DATA_SIZE,output);	
	}
	printf("\nCase 5: ");
	error_check=syscall(398,data,33,output);
	if(error_check!=0)
	{
		if(error_check == 1002)
		{	
			printf("Error: NULL pointer passed as buffer\n");
		}
		else if(error_check == 1001)
		{	
			printf("Error: Negative size passed\n");
		}
		else if(error_check == 1000)
		{
			printf("Error: Data size is less than 256");
		}
		else
		{
			perror("\nError");
		}
	}
	else
	{
		cronlog(DATA_SIZE,output);	
	}
}
