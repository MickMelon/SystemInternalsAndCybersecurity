/*
 ============================================================================
 Name        : piiotest.c
 Authors     : AR, Michael McMillan (1800833)
 Version     : 0.1
 Copyright   : See Abertay copyright notice
 Description : Test application for piio driver
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <time.h>

#include"piio.h"

/*
 * Functions for the ioctl calls
 *
 root@raspberrypi:/home/pi# ./piiotest writepin 3 1
 User App - CGI
 WRITE:Requested pin:3 - val:1 - desc:desc
 Exit 0
 root@raspberrypi:/home/pi# ./piiotest read 3
 User App - CGI
 READ:Requested  pin:3 - val:1 - desc:LKMpin
 Exit 0
 */

gpio_pin apin;
lkm_data lkmdata;
int fd = -1;

/**
 * Opens the driver file.
 */ 
int open_driver() {
	char drvPath[30];
	
	sprintf(drvPath, "//dev//%s", DEVICE_NAME);

	fd = open(drvPath, O_RDWR);
	if (fd < 0) {
		printf("ERROR: Can't open device file: %s\n", DEVICE_NAME);
		exit(-1);
	}

	return fd;
}

/**
 * Closes the driver file.
 */ 
void close_driver() {
	close(fd);
	fd = -1;
}

/**
 * Writes a message to the driver.
 */  
int write_to_driver() {
	int ret;
	fd = open_driver();

	/* Write to kernel space - see dmesg command*/
	strcpy(lkmdata.data, "This is from user application");
	lkmdata.len = 32;
	lkmdata.type = 'w';
	ret = ioctl(fd, IOCTL_PIIO_WRITE, &lkmdata);

	if (ret < 0) {
		printf("ERROR: Function failed:%d\n", ret);
		close_driver();
		return -1;
	}

	close_driver();
	return 1;
}

/**
 * Reads a message from the driver.
 */ 
int read_from_driver() {
	int ret;
	fd = open_driver();

	/*Read from kernel space - see dmesg command*/
	strcpy(lkmdata.data, "");
	ret = ioctl(fd, IOCTL_PIIO_READ, &lkmdata);

	if (ret < 0) {
		printf("ERROR: ioctl error in read_from_driver() (ret: %d)\n", ret);
		close_driver();
		return -1;
	}

	printf("Message from driver: %s\n", lkmdata.data);

	close_driver();
	return 1;
}

/**
 * Writes a value to the driver to turn a GPIO pin on/off.
 */ 
int write_to_driver_gpio(int pin, int value) {
	int ret;

	if (value < 0 || value > 1) {
		printf("ERROR: value is greater than 0 or less than 1 (value: %i) in toggle_pin()\n", value);
		return -1;
	}

	fd = open_driver();	

	/*  Pass GPIO struct with IO control */
	memset(&apin, 0, sizeof(apin));
	strcpy(apin.desc, "Writepin");
	apin.pin = pin;
	apin.value = value;
	
	/* Pass 'apin' struct to 'fd' with IO control*/
	ret = ioctl(fd, IOCTL_PIIO_GPIO_WRITE, &apin);
	printf("WRITE: Requested pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);
	
	if (ret < 0) {
		printf("ERROR: ioctl error in write_to_driver_gpio() (ret: %d)\n", ret);
		close_driver();
		return -1;
	}

	close_driver();
	return 1;
}

/**
 * Reads from the driver to see if specified GPIO pin is on or off
 */ 
int read_from_driver_gpio(int pin) {
	int ret;
	fd = open_driver();

	/*  Pass GPIO struct with IO control */
	memset(&apin , 0, sizeof(apin));
	strcpy(apin.desc, "Readpin");
	apin.pin = pin;

	/* Pass 'apin' struct to 'fd' with IO control*/
	ret = ioctl(fd, IOCTL_PIIO_GPIO_READ, &apin);
	printf("READ: Requested  pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);

	if (ret < 0) {
		printf("ERROR: ioctl error in read_from_driver_gpio() (ret: %d)\n", ret);
		close_driver();
		return -1;
	}

	close_driver();
	return apin.value;
}

/**
 * Toggles a pin on and off for the specified amount of times with
 * the specified time interval between each toggle (ms)
 */ 
int toggle_pin(int pin, int times, int ms) {
	// Get the current pin status
	int value = read_from_driver_gpio(pin);

	if (value < 0 || value > 1) {
		printf("ERROR: value is greater than 0 or less than 1 (value: %i) in toggle_pin()\n", value);
		return -1;
	} 

	// Make the value opposite of current value
	value > 0 ? value-- : value++;

	// Loop through for the specified times
	for (int i = 0; i < times; i++) {		
		// Toggle the pin on or off
		printf("TOGGLE: pin:%i - times:%i - current:%i - value:%i - ms:%i\n", pin, times, i, value, ms);		
		write_to_driver_gpio(pin, value);

		// Wait for specified time (usleep works in nanoseconds)
		usleep(ms * 1000);

		// Make the value opposite of current value
		value > 0 ? value-- : value++;
	}

	return 1;
}

/**
 * The entry point to the program. Handles the command.
 */ 
int main(int argc, char *argv[]) {
	int pin, value, times, ms;
	printf("User App\n");
	
	if (argc > 1) {
		// Check if user typed "readmsg"
		if (!strncmp(argv[1], "readmsg", 8)) {
			read_from_driver();
		}

		// Check if user typed "writemsg"
		else if (!strncmp(argv[1], "writemsg", 9)) {
			write_to_driver();
		}

		// Check if user typed "readpin"
		else if (!strncmp(argv[1], "readpin", 8)) {
			// Display message if not enough args passed
			if (argc == 3) {
				pin = strtol(argv[2], NULL, 10);
				read_from_driver_gpio(pin);
			} else {
				printf("USAGE: piio readpin [pin]\n");
			}		
		}

		// Check if user typed "writepin"
		else if (!strncmp(argv[1], "writepin", 9)) {
			// Display message if not enough args passed
			if (argc == 4) {
				pin = strtol(argv[2], NULL, 10);
				value = strtol(argv[3], NULL, 10);
				write_to_driver_gpio(pin, value);
			} else {
				printf("USAGE: piio writepin [pin] [value(1/0)]\n");
			}
		}

		// Check if user typed "toggle"
		else if (!strncmp(argv[1], "toggle", 7)) {
			if (argc == 5) {
				pin = strtol(argv[2], NULL, 10);
				times = strtol(argv[3], NULL, 10);
				ms = strtol(argv[4], NULL, 10);
				toggle_pin(pin, times, ms);
			} else {
				printf("USAGE: piio toggle [pin] [times] [ms]\n");
			}			
		}
	} else {
		// If the user didn't specify any arguments, show the usage help text.
		printf("USAGE: piio [readmsg/writemsg/readpin/writepin/toggle]\n");
	}

	printf("Exit 0\n");
	return 0;
}