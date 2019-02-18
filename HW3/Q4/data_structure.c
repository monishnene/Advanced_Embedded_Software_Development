#include <linux/unistd.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/types.h>
#define ANIMAL_NAME_SIZE 12
#define ECOSYSTEM_SIZE 50
#define HASH_TABLE_SIZE 300

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Monish Nene");
MODULE_DESCRIPTION("Kernel Module Data Structure for Ecosystem");
MODULE_VERSION("1.0");

unsigned char* hash_table;
unsigned char** unique_array;
int unique_animals;
unsigned char* animals[ECOSYSTEM_SIZE];

typedef struct
{
	unsigned char** animal;
	struct list_head list;
}node_t;

unsigned char* hash_table;

unsigned char raw_size=50,
*raw[50]={"Tiger","Dog","Cat","Elephant","Frog","Goat","Sheep","Lion","Eagle","Shark","Whale","Dolphin","Rhino","Panda","Bear","Monkey","Pig","Giraffe","Zebra","Horse","Rabbit","Wolf","Chicken","Buffalo","Panda","Bear","Monkey","Pig","Goat","Sheep","Lion","Eagle","Shark","Whale","Dolphin","Rhino","Panda","Bear","Monkey","Pig","Giraffe","Zebra","Sheep","Lion","Eagle","Shark","Whale","Dolphin","Rhino","Dolphin"};


unsigned int hash(uint8_t* str)
{
	unsigned int key=1;
	unsigned int i=0;
	while(*(str+i)!=0)
	{
		key=*(str+i)**(str+i++)*key;		
	}
	key=key%HASH_TABLE_SIZE;
	return key;
}

void link_list_operation(unsigned char** animal_type,unsigned int count)
{
	node_t *ptr;
	node_t* nodes[unique_animals];
	LIST_HEAD(my_head);
	static unsigned char none_str[]="none";
	unsigned int i=0,node_counter=0,size=0,key=0,unique_animals_copy=0;
	if(animal_type==NULL)
	{
		*animal_type=none_str;
		unique_animals_copy=unique_animals;
		for(i=0;i<unique_animals;i++)
		{
			if(*(hash_table+hash(unique_array[i]))>count)
			{
				nodes[node_counter]=(node_t*)kmalloc(sizeof(node_t*),GFP_KERNEL);
				nodes[node_counter]->animal=animal_type;
				list_add(&(nodes[node_counter]->list),&my_head);
				node_counter++;
			}
			else
			{
				unique_animals_copy--;
			}
		}
		size=unique_animals_copy;
	}
	else
	{
		size=*(hash_table+hash(*animal_type));
		if(size>count)
		{
			node_t* nodes[size];
			nodes[node_counter]=(node_t*)kmalloc(sizeof(node_t*),GFP_KERNEL);
			nodes[node_counter]->animal=animal_type;
			list_add(&(nodes[node_counter]->list),&my_head);	
			node_counter++;
			size=1; 
		}
		else
		{
			printk("\nNo animals added");
			size=0; 
		}
	}
	printk("\n\nSet 1:\nAnimals in Ecosystem\nFilter 1: %s, Filter 2: count_greater_than=%d\n",*animal_type,count);
	list_for_each_entry(ptr,&my_head,list)
	{
		printk("%s\t",*(ptr->animal));
	}
	printk("\n");
}

unsigned int bubble_sort_alphabetical(int count,unsigned char** data)
{
	unsigned char* temp;
	int i=0,j=0;
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


static int __init data_structure_init(void)
{
	unsigned int i=0;
	unsigned char* ptr;
	unique_animals=raw_size-1;
	unique_array=(unsigned char**)kmalloc(unique_animals*sizeof(unsigned char*),GFP_KERNEL);
	hash_table =(unsigned char*)kmalloc(unique_animals,GFP_KERNEL);
	for(i=1;i<raw_size;i++)
	{
		*(unique_array+i-1)=(unsigned char*)kmalloc(ANIMAL_NAME_SIZE,GFP_KERNEL);
		*(unique_array+i-1)=strcpy(*(unique_array+i-1),raw[i]);
	}
	unique_animals=bubble_sort_alphabetical(unique_animals,unique_array);
	printk("\nUnique Animals in the Ecosystem are\n");
	for(i=0;i<unique_animals;i++)
	{
		printk("%s\t",unique_array[i]);
		*(hash_table+hash(unique_array[i]))=0;
	}
	printk("\n\nAnimals in the Seed Array are\n");
	for(i=0;i<ECOSYSTEM_SIZE;i++)
	{
		animals[i]=(unsigned char*)kmalloc(ANIMAL_NAME_SIZE,GFP_KERNEL);
		animals[i]=strcpy(animals[i],unique_array[i]);
		printk("%s\t",animals[i]);
		*(hash_table+hash(animals[i]));
	}
	printk("\n\nSet 1:\nAnimals in Ecosystem\n");
	for(i=0;i<unique_animals;i++)
	{
		printk("%s=%d\t",unique_array[i],*(hash_table+hash(unique_array[i])));
	}
	printk("\n\nData Structure used is Hash Table\nSize of each node = %ld bytes and total nodes = %d\nTotal Space Allocated = %ld bytes\n",sizeof(unsigned char*),unique_animals,sizeof(unsigned char*)*unique_animals);
	link_list_operation(NULL,0);
	link_list_operation(&unique_array[10],0);
	link_list_operation(NULL,2);
	link_list_operation(&unique_array[40],2);
	return 0;
}

static void __exit data_structure_exit(void)
{
	int i=0; 
	for(i=0;i<ECOSYSTEM_SIZE;i++)
	{
		kfree(animals[i]);
	}
	for(i=0;i<unique_animals;i++)
	{
		kfree(unique_array[i]);
	}
	kfree(hash_table);
}

module_init(data_structure_init);
module_exit(data_structure_exit);

