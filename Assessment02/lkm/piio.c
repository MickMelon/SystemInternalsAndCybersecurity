/*
 ============================================================================
 Name        : piio.c
 Author      : Dr Abdul Razaq
 Version     : 0.1
 Copyright   : See Abertay copyright notice
 Description : RPi Zero Wireless - GPIO Driver
 ============================================================================
 */
#include "piio.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/seq_file.h>
#include <linux/spinlock.h>

#define DEFINE_SPINLOCK(x) spinlock_t x = __SPIN_LOCK_UNLOCKED(x)

DEFINE_SPINLOCK(piio_spinlock);

static int MajorNum = 100;
static struct class*  ClassName  = NULL;
static struct device* DeviceName = NULL;

lkm_data lkmdata;
gpio_pin apin;

/**
 * Performs initialisation for later operations
 */ 
static int device_open(struct inode *inode, struct file *file)
{
	spin_lock(&piio_spinlock);
	printk(KERN_INFO "device_open(%p)\n", file);
	try_module_get(THIS_MODULE);	
	return 0;
}

/**
 * This function is intended to deallocate anything that is open and shut down
 * the device.
 */ 
static int device_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO " device_release(%p)\n", file);
	module_put(THIS_MODULE);
	spin_unlock(&piio_spinlock);
	return 0;
}

/**
 * Handles the ioctl() function calls from user space application.
 */ 
static int 		device_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
	int i, err;
	char *temp;
	char ch;

	printk("device_ioctl: Device IOCTL invoked : 0x%x - %u\n" , cmd , cmd);

	// Check if the command is valid
	if (_IOC_TYPE(cmd) != CMD_MAGIC_NUMBER) return -ENOTTY;
	if (_IOC_NR(cmd) > CMD_MAX_NUMBER) return -ENOTTY;

	// Verify read and write
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;

	switch (cmd) {
		// Handle read
		case IOCTL_PIIO_READ:
			strcpy(lkmdata.data ,"This is from lkm\0");
			lkmdata.len = 101;
			lkmdata.type = 'r';
			copy_to_user((void *)arg, &lkmdata, sizeof(lkm_data));
			printk("Device IOCTL WRITE\n");
			break;

		// Handle write
		case IOCTL_PIIO_WRITE:
			printk("Device IOCTL READ\n");
			copy_from_user(&lkmdata, (lkm_data *)arg, sizeof(lkm_data));
			printk("From user: %s - %u - %c\n" , lkmdata.data , lkmdata.len , lkmdata.type);
			break;

		// Handle read GPIO
		case IOCTL_PIIO_GPIO_READ:
			memset(&apin , 0, sizeof(apin));
			copy_from_user(&apin, (gpio_pin *)arg, sizeof(gpio_pin));
			gpio_request(apin.pin, apin.desc);
			apin.value = gpio_get_value(apin.pin);
			strcpy(apin.desc, "LKMpin");
			copy_to_user((void *)arg, &apin, sizeof(gpio_pin));

			printk("piio IOCTL_PIIO_GPIO_READ: pi:%u - val:%i - desc:%s\n" , apin.pin , apin.value , apin.desc);
			break;
			
		// Handle write GPIO
		case IOCTL_PIIO_GPIO_WRITE:
			copy_from_user(&apin, (gpio_pin *)arg, sizeof(gpio_pin));
			gpio_request(apin.pin, apin.desc);
			gpio_direction_output(apin.pin, 0);
			gpio_set_value(apin.pin, apin.value);

			printk("piio IOCTL_PIIO_GPIO_WRITE: pi:%u - val:%i - desc:%s\n" , apin.pin , apin.value , apin.desc);
			break;

		default:
			printk("piio: command format error\n");
			return -ENOTTY;
	}

	return 0;
}

/**
 * Defines the functions used for file operations
 */
struct file_operations Fops = {
	.unlocked_ioctl = device_ioctl,
	.open = device_open,
	.release = device_release,
};

/**
 * Called when the module is installed (ie when insmod is executed)
 */ 
static int __init piio_init(void){
	int ret_val;
	ret_val = 0;

	printk(KERN_INFO "piio: Initializing the piio\n");
	MajorNum = register_chrdev(0, DEVICE_NAME, &Fops);
		if (MajorNum<0){
			printk(KERN_ALERT "piio failed to register a major number\n");
			return MajorNum;
		}
	printk(KERN_INFO "piio: registered with major number %d\n", MajorNum);

	ClassName = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(ClassName)){
		unregister_chrdev(MajorNum, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(ClassName);
	}
	printk(KERN_INFO "piio: device class registered\n");

	DeviceName = device_create(ClassName, NULL, MKDEV(MajorNum, 0), NULL, DEVICE_NAME);
	if (IS_ERR(DeviceName)){
		class_destroy(ClassName);
		unregister_chrdev(MajorNum, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create the device\n");
		return PTR_ERR(DeviceName);
	}
	printk(KERN_INFO "piio: device class created\n");

	return 0;
}

/**
 * Called when the module is removed (ie when rmmod is executed)
 */ 
static void __exit piio_exit(void){
	device_destroy(ClassName, MKDEV(MajorNum, 0));
	class_unregister(ClassName);
	class_destroy(ClassName);
	unregister_chrdev(MajorNum, DEVICE_NAME);
	gpio_free(apin.pin);
	printk(KERN_INFO "piio: Module removed\n");
}

/**
 * Kernel module information
 */ 
module_init(piio_init);
module_exit(piio_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("CMP408 - Michael McMillan 1800833");
MODULE_DESCRIPTION("Prototype char kernel module to read from and write to GPIO pins.");
MODULE_VERSION("0.1");