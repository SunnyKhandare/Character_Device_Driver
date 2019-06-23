#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

static int __init first(void);
static void __exit last(void);
static int Device_open_sk(struct inode * inode , struct file * file);
static int Device_release_sk(struct inode * inode , struct file * file);
static ssize_t Device_read_sk(struct file * filep , char * buffer , size_t length , loff_t * offset);

static int Major_Number = 0;
static int device_open = 0;
static char msg[100];
static char * msg_ptr = msg;
static struct file_operations fops={
					read : Device_read_sk,
					open : Device_open_sk,
					release : Device_release_sk
				   }; // This is the step to register the functions our module provides as an device driver.So when ever any program uses this module it can call the functions Device_read_sk to read, Device_open_sk to open , Device_release_sk to close or release. Here the program can use only these three functions as we have only provided this three functions.We can also write many more functions which will provide functionality to the programs using out module , and for that we have to register that function to the member of object of file_operations.
 

static int __init first(void)
{
	Major_Number= register_chrdev(0, "char_D_D" , &fops);
	
	printk("Device Driver Register Successfully with Major Number %d\n",Major_Number);

	return 0;
}

static void __exit last(void)
{
	unregister_chrdev( Major_Number , "char_D_D");

	printk("Device driver unregistered successfully\n");
}

/*

	To use our module as an device driver , program using our module should first call the function registered with "open" member of object of file_operations.
	And then it can call other functions to get the service of our device driver.
	After the use of device drivers functionality , program should release the driver by calling the function registered with "release" member of object of file_operations , otherwise We cant ever unload our module(i.e device driver) 

*/

static int Device_open_sk(struct inode * inode , struct file * file)
{
	if(device_open) // This condition check whether device file is currently in use by any other program . If yes then function returns , otherwise device file is opened.
		return -EBUSY;

	sprintf(msg,"Heyyyy This is the message");
	msg_ptr=msg;
	try_module_get(THIS_MODULE); // Increment the use count.
	device_open=1;
	return 1;
}

static ssize_t Device_read_sk(struct file * filep, char * buffer , size_t length , loff_t * offset)
{
	int bytes_read=0;

	if(*msg_ptr)
		return 0;

	while(length && *msg_ptr)
	{
		put_user(*(msg_ptr++), buffer++);
		length--;
		bytes_read++;
	}

	return bytes_read;
}

static int Device_release_sk(struct inode * inode , struct file * file)
{
	device_open=0;
	module_put(THIS_MODULE); // Decrement the use count.

	return 0;
}

module_init(first);
module_exit(last);
