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

/**
 * Checks if a pin is a valid GPIO pin.
 */ 
int isValidPin(int pin) {
    int validPins[28] = {3, 5, 7, 11, 13, 15, 19, 21, 23, 27, 29, 31, 33, 35, 37, 8, 10, 12, 16, 18,
        22, 24, 26, 28, 32, 36, 38, 40};

    for (int i = 0; i < 28; i++) {
        if (validPins[i] == pin) {
            return 1;
        }
    }

    return 0;
}

/**
 * Writes a message to the driver.
 */  
int writeToDriver(int fd) {
	int ret;
	/* Write to kernel space - see dmesg command*/
	strcpy(lkmdata.data, "This is from user application");
	lkmdata.len = 32;
	lkmdata.type = 'w';
	ret = ioctl(fd, IOCTL_PIIO_WRITE, &lkmdata);

	if (ret < 0) {
		printf("Function failed:%d\n", ret);
		exit(-1);
	}

}

/**
 * Reads a message from the driver.
 */ 
int readFromDriver(int fd) {
	int ret;

	/*Read from kernel space - see dmesg command*/
	strcpy(lkmdata.data, "");
	ret = ioctl(fd, IOCTL_PIIO_READ, &lkmdata);

	if (ret < 0) {
		printf("ERROR: ioctl error in readFromDriver() (ret: %d)\n", ret);
		exit(-1);
	}

	printf("Message from driver: %s\n", lkmdata.data);
}

/**
 * Writes a value to the driver to turn a GPIO pin on/off.
 */ 
int writeToDriverGpio(int fd, int pin, int value) {
	int ret;

	if (value < 0 || value > 1) {
		printf("ERROR: value is greater than 0 or less than 1 (value: %i) in togglePin()\n", value);
		return -1;
	}

	if (!isValidPin(pin)) {
		printf("ERROR: Specified pin is not a valid GPIO pin (%d)", pin);
		return -1;
	}

	/*  Pass GPIO struct with IO control */
	memset(&apin, 0, sizeof(apin));
	strcpy(apin.desc, "Writepin");
	apin.pin = pin;
	apin.value = value;
	
	/* Pass 'apin' struct to 'fd' with IO control*/
	ret = ioctl(fd, IOCTL_PIIO_GPIO_WRITE, &apin);
	printf("WRITE: Requested pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);
	
	if (ret < 0) {
		printf("ERROR: ioctl error in writeToDriverGpio() (ret: %d)\n", ret);
		return -1;
	}

	return 1;
}

/**
 * Reads from the driver to see if specified GPIO pin is on or off
 */ 
int readFromDriverGpio(int fd, int pin) {
	int ret;

	if (!isValidPin(pin)) {
		printf("ERROR: Specified pin is not a valid GPIO pin (%d)", pin);
		return -1;
	}

	/*  Pass GPIO struct with IO control */
	memset(&apin , 0, sizeof(apin));
	strcpy(apin.desc, "Readpin");
	apin.pin = pin;

	/* Pass 'apin' struct to 'fd' with IO control*/
	ret = ioctl(fd, IOCTL_PIIO_GPIO_READ, &apin);
	printf("READ: Requested  pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);

	if (ret < 0) {
		printf("ERROR: ioctl error in readFromDriverGpio() (ret: %d)\n", ret);
		return -1;
	}

	return apin.value;
}

/**
 * Toggles a pin on and off for the specified amount of times with
 * the specified time interval between each toggle (ms)
 */ 
int togglePin(int fd, int pin, int times, int ms) {
	// Get the current pin status
	int value = readFromDriverGpio(fd, pin);

	if (value < 0 || value > 1) {
		printf("ERROR: value is greater than 0 or less than 1 (value: %i) in togglePin()\n", value);
		return -1;
	} 

	// Make the value opposite of current value
	value = value == 1 ? 0 : 1;

	// Loop through for the specified times
	for (int i = 0; i < times; i++) {		
		// Toggle the pin on or off
		printf("TOGGLE: pin:%i - times:%i - current:%i - value:%i - ms:%i\n", pin, times, i, value, ms);		
		writeToDriverGpio(fd, pin, value);

		// Wait for specified time (usleep works in nanoseconds)
		usleep(ms * 1000);

		// Make the value opposite of current value
		value = value == 1 ? 0 : 1;
	}

	return 1;
}

/**
 * Displays the menu
 */
void showMenu(int argc, char *argv[], int fd) {
	if (argc > 1) {
		// Check if user typed "readmsg"
		if (!strncmp(argv[1], "readmsg", 8)) {
			readFromDriver(fd);
		}

		// Check if user typed "writemsg"
		else if (!strncmp(argv[1], "writemsg", 9)) {
			writeToDriver(fd);
		}

		// Check if user typed "readpin"
		else if (!strncmp(argv[1], "readpin", 8)) {
			// Display message if not enough args passed
			if (argc < 3) {
				printf("USAGE: piio readpin [pin]\n");
				return;
			}

			int pin = strtol(argv[2], NULL, 10);
			readFromDriverGpio(fd, pin);
		}

		// Check if user typed "writepin"
		else if (!strncmp(argv[1], "writepin", 9)) {
			// Display message if not enough args passed
			if (argc < 4) {
				printf("USAGE: piio writepin [pin] [value(1/0)]\n");
				return;
			}

			int pin = strtol(argv[2], NULL, 10);
			int value = strtol(argv[3], NULL, 10);
			writeToDriverGpio(fd, pin, value);
		}

		// Check if user typed "toggle"
		else if (!strncmp(argv[1], "toggle", 7)) {
			// Display message if not enough args passed
			if (argc < 5) {
				printf("USAGE: piio toggle [pin] [times] [ms]\n");
				return;
			}

			int pin = strtol(argv[2], NULL, 10);
			int times = strtol(argv[3], NULL, 10);
			int ms = strtol(argv[4], NULL, 10);
			togglePin(fd, pin, times, ms);
		}
	} else {
		// If the user didn't specify any arguments, show the usage help text.
		printf("USAGE: piio [readmsg/writemsg/readpin/writepin/toggle]\n");
	}
}

/**
 * The entry point to the program. Attempts to load the driver and then shows the menu.
 */ 
int main(int argc, char *argv[]) {
	

	printf("User App\n");
	int fd, ret;
	char *msg = "Message passed by ioctl\n";
	char drvPath[30];
	
	sprintf(drvPath, "//dev//%s", DEVICE_NAME);

	fd = open(drvPath, O_RDWR);
	if (fd < 0) {
		printf("ERROR: Can't open device file: %s (have you run as sudo?)\n", DEVICE_NAME);
		exit(-1);
	}

	showMenu(argc, argv, fd);

	printf("Exit 0\n");

	close(fd);
	return 0;
}