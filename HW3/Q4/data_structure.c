#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <search.h>
#define ANIMAL_NAME_SIZE 12
#define ARRAY_SIZE 50
#define HASH_TABLE_SIZE (uint32_t)(ARRAY_SIZE*log(ARRAY_SIZE))

uint8_t* hash_table;

uint32_t bubble_sort_alphabetical(int32_t count,uint8_t** data)
{
	uint8_t* temp;
	int32_t i=0,j=0;
	for(i=0;i<count;i++)
	{
		for(j=i;j<count;j++)
		{
			if(strcmp(*(data+i),*(data+j))>0)
        		{
            			temp=*(data+i);
            			*(data+i)=*(data+j);
            			*(data+j)=temp;
        		}
		}
	}
	for(i=0;i<count-1;i++)
	{
		if(strcmp(*(data+i),*(data+i+1))==0)
		{
			for(j=i;j<count-1;j++)
			{	
				*(data+j)=*(data+j+1);
        		}
			count--;
			i--;		
		}
	}
	return count;
}


void main(int32_t argc, uint8_t **argv)
{
	if(argc<2)
	{
		printf("./data_structure.elf <animals>\n");
		exit(0);
	}
	uint32_t i=0;
	uint8_t* ptr;
	int32_t unique_animals=argc-1;
	uint8_t** unique_array=(uint8_t**)malloc(unique_animals*sizeof(uint8_t*));
	ENTRY e,*ep;
	srand(time(NULL));
	uint8_t* animals[ARRAY_SIZE];
	hash_table =(uint8_t*)calloc(unique_animals,1);
	hcreate(HASH_TABLE_SIZE);
	for(i=1;i<argc;i++)
	{
		*(unique_array+i-1)=(uint8_t*)malloc(ANIMAL_NAME_SIZE);
		*(unique_array+i-1)=strcpy(*(unique_array+i-1),argv[i]);
	}
	unique_animals=bubble_sort_alphabetical(unique_animals,unique_array);
	printf("\nUnique Animals in the Ecosystem are\n");
	for(i=0;i<unique_animals;i++)
	{
		printf("%s\t",unique_array[i]);
		e.key=unique_array[i];
		ep=hsearch(e,ENTER);
		if(ep==NULL)
		{
			fprintf(stderr,"Hash Entry Failed\n");
			exit(EXIT_FAILURE);	
		}
		ep->data=(void*)(hash_table+i);
	}
	printf("\n\nAnimals in the Seed Array are\n");
	for(i=0;i<ARRAY_SIZE;i++)
	{
		animals[i]=(uint8_t*)malloc(ANIMAL_NAME_SIZE);
		animals[i]=strcpy(animals[i],unique_array[rand()%unique_animals]);
		printf("%s\t",animals[i]);
		e.key=animals[i];
		ep=hsearch(e,ENTER);
		if(ep==NULL)
		{
			fprintf(stderr,"Hash Entry Failed\n");
			exit(EXIT_FAILURE);	
		}
		ptr=(uint8_t*)(ep->data);
		*(ptr)+=1;
	}
	i=1;	
	printf("\n");
	printf("\nAnimals in Ecosystem\n");
	for(i=0;i<unique_animals;i++)
	{
		e.key=unique_array[i];
		ep=hsearch(e,ENTER);
		if(ep==NULL)
		{
			fprintf(stderr,"Hash Entry Failed\n");
			exit(EXIT_FAILURE);	
		}
		printf("%s=%d\n",unique_array[i],*(uint8_t*)(ep->data));
		
	}
	hdestroy();
	for(i=0;i<ARRAY_SIZE;i++)
	{
		free(animals[i]);
	}
	for(i=0;i<unique_animals;i++)
	{
		free(unique_array[i]);
	}
	free(hash_table);
	exit(EXIT_SUCCESS);
}

