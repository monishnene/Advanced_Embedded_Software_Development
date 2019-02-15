#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define ARRAY_SIZE 50
#define HASH_TABLE_SIZE (uint32_t)(ARRAY_SIZE*log(ARRAY_SIZE))

uint8_t* hash_table;

uint32_t hash(uint8_t* str)
{
	uint32_t key=1,i=0;
	while(*(str+i)!=0)
	{
		key=*(str+i)**(str+i++)*key;		
	}
	key=key%HASH_TABLE_SIZE;
	return key;
}

void main(int32_t argc, uint8_t **argv)
{
	uint32_t i=0;
	srand(time(NULL));
	uint8_t* animals[ARRAY_SIZE];
	hash_table = calloc(HASH_TABLE_SIZE,1);
	for(i=0;i<ARRAY_SIZE;i++)
	{
		animals[i]=argv[(rand()%(argc-1))+1];	
	}
	for(i=0;i<ARRAY_SIZE;i++)
	{
		printf("%s ",animals[i]);
		*(hash_table+hash(animals[i]))+=1;
	}
	printf("\nHash Values\n");
	i=1;	
	while(i<argc)
	{
		printf("%s=%d key = %d\n",argv[i],*(hash_table+hash(argv[i])),hash(argv[i]));
		i++;
	}
	free(hash_table);
	printf("\n");
}

