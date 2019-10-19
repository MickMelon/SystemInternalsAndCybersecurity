/*
 ============================================================================
 Name        : piiotest.c
 Author      : AR
 Version     :
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
 * https://www.geeksforgeeks.org/time-delay-c/
 */ 
void delay(int ms) {
	clock_t start_time = clock();
	while (clock() < (start_time + ms));
}

void write_to_driver(int fd) {
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

void read_from_drvier(int fd) {
	int ret;

	/*Read from kernel space - see dmesg command*/
	strcpy(lkmdata.data, "");
	ret = ioctl(fd, IOCTL_PIIO_READ, &lkmdata);

	if (ret < 0) {
		printf("Function failed:%d\n", ret);
		exit(-1);
	}

	printf("Message from driver: %s\n", lkmdata.data);
}

void write_to_driver_gpio(int fd, int pin, int value) {
	int ret;

	/*  Pass GPIO struct with IO control */
	memset(&apin , 0, sizeof(apin));
	strcpy(apin.desc, "Writepin");
	apin.pin = pin;
	apin.value = value;

	if (apin.value == 1 || apin.value == 0) {
		/* Pass 'apin' struct to 'fd' with IO control*/
		ret = ioctl(fd, IOCTL_PIIO_GPIO_WRITE, &apin);
		printf("WRITE:Requested pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);
		if (ret < 0) {
			printf("It fucked up\n");
		}
	} else {
		printf("value wrong\n");
	}		
}

int read_from_driver_gpio(int fd, int pin) {
	int ret;

	/*  Pass GPIO struct with IO control */
	memset(&apin , 0, sizeof(apin));
	strcpy(apin.desc, "Readpin");
	apin.pin = pin;
	/* Pass 'apin' struct to 'fd' with IO control*/
	ret = ioctl(fd, IOCTL_PIIO_GPIO_READ, &apin);
	printf("READ:Requested  pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);

	if (ret < 0) {
		printf("It fucked up\n");
		return -1;
	}

	return apin.value;
}

int main(int argc, char *argv[]) {
	printf("User App\n");
	int fd, ret;
	char *msg = "Message passed by ioctl\n";
	char drvPath[30];
	
	sprintf(drvPath, "//dev//%s", DEVICE_NAME);

	fd = open(drvPath, O_RDWR);
	if (fd < 0) {
		printf("Can't open device file: %s\n", DEVICE_NAME);
		exit(-1);
	}


	if (argc > 1) {
		if (!strncmp(argv[1], "readmsg", 8)) {
			read_from_drvier(fd);
		}

		if (!strncmp(argv[1], "writemsg", 9)) {
			write_to_driver(fd);
		}

		if (!strncmp(argv[1], "readpin", 8)) {
			int pin = strtol(argv[2], NULL, 10);

			read_from_driver_gpio(fd, pin);
		}

		if (!strncmp(argv[1], "writepin", 9)) {
			int pin = strtol(argv[2], NULL, 10);
			int value = strtol(argv[3], NULL, 10);

			write_to_driver_gpio(fd, pin, value);
		}

		if (!strncmp(argv[1], "toggle", 7)) {
			int pin = strtol(argv[2], NULL, 10);
			int times = strtol(argv[3], NULL, 10);
			int ms = strtol(argv[4], NULL, 10);

			int value = read_from_driver_gpio(fd, pin);
			int newValue;
			if (value < 0) {
				printf("error\n");
			} else {
				if (value == 1) newValue = 0;
				else newValue = 1;

				for (int i = 0; i < times; i++) {		
					printf("TOGGLE: pin:%i times:%i current:%i value:%i ms:%i\n", pin, times, i, newValue, ms);		
					write_to_driver_gpio(fd, pin, newValue);
					usleep(ms * 1000);

					if (newValue == 1) newValue = 0;
					else newValue = 1;
				}			
			}
		}
	} else {
		printf("USAGE: ./run [readmsg/writemsg/readpin/writepin/toggle]\n");
	}

	printf("Exit 0\n");

	close(fd);
	return 0;
}