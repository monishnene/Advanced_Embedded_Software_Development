/***********************************************************************
 * fileio.c
 * Advanced Embedded Software Development
 * Author: Monish Nene
 * Date: 02/04/2018
 * @brief This file has the functions for file operations
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#define STRING_SIZE 100
#define SPACE 32
#define FILENAME_SIZE 20
#define READONLY 444
#define WRITEONLY 222
#define READWRITE 666

/* Permission codes user-group-ownership
0 = ---
1 = --x
2 = -w-
3 = -wx
4 = r--
5 = r-x
6 = rw-
7 = rwx
*/

void file_write(uint8_t* filename, uint8_t* buffer, int32_t data_bytes)
{
	FILE* fptr=fopen(filename, "w");
	fwrite(buffer, 1, data_bytes, fptr);
	fclose(fptr);
	return;
}

void file_append(uint8_t* filename, uint8_t* buffer, int32_t data_bytes)
{
	FILE* fptr=fopen(filename, "a");
	fwrite(buffer, 1, data_bytes, fptr);
	fclose(fptr);
	return;
}

void modify_permissions(uint8_t* filename,uint16_t permit)
{
	uint8_t* command = (uint8_t*) malloc(STRING_SIZE);
	sprintf(command,"chmod %d %s",permit,filename);
	system(command);
	free(command);
}

void flush_file(uint8_t* filename)
{	
	FILE* fptr=fopen(filename, "r");
	uint32_t i=0,file_size=0;
	if(fptr!=NULL)
	{
		printf("Flushing data of file %s\n",filename);
		fseek(fptr,0,SEEK_END);
		file_size=ftell(fptr);
		fseek(fptr,0,SEEK_SET);
		uint8_t* buffer = (uint8_t*)malloc(file_size);
		i=fread(buffer,1,file_size,fptr);
		for(i=0;i<file_size;i++)
		{
			putchar(*(buffer+i));
		}
		free(buffer);
		fclose(fptr);
	}
	else
	{
		printf("File %s not found\n",filename);
	}
	return;
}

void read_char(uint8_t* filename,uint8_t* buffer)
{	
	FILE* fptr=fopen(filename, "r");	
	uint32_t i=0,file_size=0;
	if(fptr!=NULL)
	{
		i=fread(buffer,1,1,fptr);
	}
	else
	{
		printf("File %s not found\n",filename);
	}
	fclose(fptr);
	return;
}

void read_str(uint8_t* filename,uint8_t* buffer)
{	
	FILE* fptr=fopen(filename, "r");
	uint32_t i=0,file_size=0;
	if(fptr!=NULL)
	{
		fseek(fptr,0,SEEK_END);
		file_size=ftell(fptr);
		fseek(fptr,0,SEEK_SET);
		uint8_t* temp = (uint8_t*)malloc(file_size);
		i=fread(temp,1,file_size,fptr);
		while(*(temp+i)!=0)
		{
			*(buffer+i)=*(temp+i++);
		}
	}
	else
	{
		printf("File %s not found\n",filename);
	}
	fclose(fptr);
	return;
}
	
int32_t main(int32_t argc, uint8_t **argv)
{
	uint8_t i=0,j=0,counter=0,temp_char=0; 
	uint8_t* str = (uint8_t*) malloc(STRING_SIZE);
	uint8_t *filename,*buffer;	
	uint8_t* data_read = (uint8_t*) malloc(STRING_SIZE);
	if(argc==1)
	{
		printf("Enter a string\n");
		scanf("%s",str);
		*(str+strlen(str))=SPACE;
	}
	for(i=1;i<argc;i++)
	{
		j=0;
		while(*(argv[i]+j)!=0)	
		{
			*(str+counter++)=*(argv[i]+j++);
		}	
		*(str+counter++)=SPACE;
	}
	buffer="This string is appended\n";
	filename="test.txt";
	printf("Input string is %s\n",str);
	file_write(filename,str,strlen(str));	
	modify_permissions(filename,WRITEONLY);	
	file_append(filename,buffer,strlen(buffer));
	modify_permissions(filename,READONLY);
	read_char(filename,&temp_char);
	printf("read char output = %c\n",temp_char);
	read_str(filename,buffer);
	printf("read string output = %s\n",buffer);
	flush_file(filename);	
	modify_permissions(filename,READWRITE);
	free(str);
	free(data_read);
	return 0;
}
