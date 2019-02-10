/***********************************************************************
 * fileio.c
 *
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
#define EXECUTE 111
#define ALL 777

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
	
int32_t main(int32_t argc, uint8_t **argv)
{
	uint8_t i=0,j=0,counter=0,temp_char=0; 
	uint8_t* str = (uint8_t*) malloc(STRING_SIZE);
	uint8_t *filename,*buffer;
	FILE* fptr;
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
	file_write(filename,str,strlen(str));	
	modify_permissions(filename,WRITEONLY);	
	file_append(filename,buffer,strlen(buffer));
	fptr=fopen(filename, "r");
	modify_permissions(filename,READONLY);
	temp_char=(uint8_t)fgetc(fptr);
	printf("read char output = %c\n",temp_char);	
	str=fgets(str,STRING_SIZE,fptr);
	printf("read string output = %s\n",str);
	modify_permissions(filename,READWRITE);
	fflush(stdin);
	fclose(fptr);
	free(str);
	free(data_read);
	return 0;
}
