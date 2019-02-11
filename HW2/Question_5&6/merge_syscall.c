void recursive_merge_sort(int count,int* data)
{
    int temp=0,middle=count/2,i=0,j=0,temp_counter=0;
    int* temp_ptr = (int*)kmalloc(count*sizeof(int),GFP_KERNEL);
    if(temp_ptr==NULL)
    {
	printk("kmalloc failed in recursion");
	return;
    }
    if((count==1)||(count==0))
    {
        return;
    }
    if(count==2)
    {
        if(*(data) < *(data+1))
        {
            temp=*(data);
            *(data)=*(data+1);
            *(data+1)=temp;
        }
        return;
    }
    recursive_merge_sort(middle,data);
    recursive_merge_sort(count-middle,data+middle);
    j=middle;
    while(temp_counter<count)
    {
	if(j>=count)
	{
		*(temp_ptr+temp_counter++)=*(data+i);
		i++;
	}
	else if(i>=middle)
	{
		*(temp_ptr+temp_counter++)=*(data+j);
		j++;
	}
	else if((*(data+i) > *(data+j)))
	{
		*(temp_ptr+temp_counter++)=*(data+i);
		i++;
	}
	else
	{
		*(temp_ptr+temp_counter++)=*(data+j);
		j++;
	}
    }
    temp_counter=0;
    while(temp_counter<count)
    {
	*(data+temp_counter)=*(temp_ptr+temp_counter);
    	temp_counter++;
    }
    return;
}

SYSCALL_DEFINE3(merge_sort,int __user *, buffer, int, size, int __user *, sorted_buffer)
{
	
	if(size < 0)
	{
		printk(KERN_ERR "Negative size allocated");
		return -1001;
	}	
	if((buffer==NULL)||(sorted_buffer==NULL))
	{
		printk(KERN_ERR "NULL pointer buffers passed");
		return -1002;
	}		
	if(size<256)
	{
		printk(KERN_ERR "Data size is less than 256");
		return -1000;
	}
	int* kbuffer = (int*)kmalloc(size*sizeof(int),GFP_KERNEL);
	int error_check=0;
	printk("Entering sys_merge_sort\n");
	if(kbuffer==NULL)
    	{
		printk(KERN_ERR "kmalloc failed in first try");
		return -3;
    	}
	printk("The input buffer is as follows\n");
	error_check=copy_from_user(kbuffer,buffer,size*sizeof(int));
	if(error_check)
	{
		return error_check;
	}
	recursive_merge_sort(size,kbuffer);
	error_check=copy_to_user(sorted_buffer,kbuffer,size*sizeof(int));	
	if(error_check)
	{
		return error_check;
	}	
	printk("Exiting sys_merge_sort\n");
	return 0;
}
