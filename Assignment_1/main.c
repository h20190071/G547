#include<linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include<linux/random.h>
#include<linux/uaccess.h>
#include<linux/version.h>
#include<linux/moduleparam.h>
#include<linux/ioctl.h>

#include<linux/slab.h>


  
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)


int32_t value1, value2;
static int c;
uint16_t number, temp;
  

static dev_t first; // variable for device number
static struct cdev c_dev; // variable for the character device structure
static struct class *cls; // variable for the device class

static long etx_ioctl(struct file *f, unsigned int ioctl_num, unsigned long ioctl_param);



 
/*****************************************************************************
STEP 4 
my_close(), my_open(), my_read(), my_write() functions are defined here
these functions will be called for close, open, read and write system calls respectively. 
*****************************************************************************/
 
static int my_open(struct inode *i, struct file *f)
{
    printk(KERN_INFO "adc0 : open()\n");
    return 0;
}
 
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "adc0 : close()\n");
    return 0;
}
 
static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
   char input1[11]="0000000000";
   char input2[17];
   int i=0, j;
   get_random_bytes(&number, sizeof(number));
   number=(number%1024);
   temp=number;
   printk(" random number generated is %d",temp);


while (number != 0) 
	{ 
		// storing remainder in binary array

		input1[9-i] = (number % 2) + '0'; 
		number = number / 2; 
		i++; 
    	}  

	if(value2 == 0)			//0-9 bits
	{
		for(j = 0;j < 10; j++)
		{
	          input2[j] = input1[j];
;		}
		
		for(j = 10;j < 16; j++)
		{
	          input2[j] = 0 + '0';
		}
		
		input2[16] = '\0';
		

		printk("Binary value = %s\n", input1);
		printk("Aligned value = %s\n", input2);

		if(copy_to_user(buf,input2,sizeof(input2)));
		return sizeof(input2);
	}
	
	else if(value2 == 1)
	{	
		for(j = 6;j < 16; j++)
		{
	          input2[j] = input1[j-6];
;		}
		
		for(j = 0;j < 6; j++)
		{
	          input2[j] = 0 + '0';
		}

		input2[16] = '\0';
		
		printk("Binary value = %s\n", input1);
		printk("Aligned value = %s\n", input2);

		if(copy_to_user(buf,input2,sizeof(input2)));
		return sizeof(input2);
	}
	
	else 
		return 0;
}





	

 


static long etx_ioctl(struct file *f, unsigned int ioctl_num, unsigned long ioctl_param)
{
   
   switch (ioctl_num)
   {
    
      case WR_VALUE:
		copy_from_user(&value1, (int32_t*) ioctl_param, sizeof (value1));
                printk(KERN_INFO "input is %d \n", value1);
		if(c==1)
			value2=value1;
		c=1;
		break;	
   }
return 0;

}
        
//###########################################################################################
 
 
static struct file_operations fops =
					{
  						.owner    = THIS_MODULE,
  						.open     = my_open,
  						.release  = my_close,
  						.read     = my_read,
 					        .unlocked_ioctl=etx_ioctl
					};
  
//########## INITIALIZATION FUNCTION ##################
// STEP 1,2 & 3 are to be executed in this function ### 
static int __init mychar_init(void) 
{
    printk(KERN_INFO " adc8 driver registered");
     
    // STEP 1 : reserve <major, minor>
    if (alloc_chrdev_region(&first, 0, 1, "adc_ioctl") < 0)
    {
        return -1;
    }
     
    // STEP 2 : dynamically create device node in /dev directory
    if ((cls = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(first, 1); 
        return -1;
    }
    if (device_create(cls, NULL, first, NULL, "adc8") == NULL)
    {
        class_destroy(cls);
        unregister_chrdev_region(first, 1);
        return -1;
    }
     
    // STEP 3 : Link fops and cdev to device node
    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, first, 1) == -1)
    {
        device_destroy(cls, first);
        class_destroy(cls);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    return 0;
}
  
static void __exit mychar_exit(void) 
{
    cdev_del(&c_dev);
    device_destroy(cls, first);
    class_destroy(cls);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Bye: adc8 driver unregistered\n\n");
}
  
module_init(mychar_init);
module_exit(mychar_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Akanksha Bajpai");
MODULE_DESCRIPTION("Character device driver for adc8");


