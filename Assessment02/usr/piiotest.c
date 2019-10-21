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

int write_to_driver(int fd) {
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

int read_from_driver(int fd) {
	int ret;

	/*Read from kernel space - see dmesg command*/
	strcpy(lkmdata.data, "");
	ret = ioctl(fd, IOCTL_PIIO_READ, &lkmdata);

	if (ret < 0) {
		printf("ERROR: ioctl error in read_from_driver() (ret: %d)\n", ret);
		exit(-1);
	}

	printf("Message from driver: %s\n", lkmdata.data);
}

int write_to_driver_gpio(int fd, int pin, int value) {
	int ret;

	if (value < 0 || value > 1) {
		printf("ERROR: value is greater than 0 or less than 1 (value: %i) in toggle_pin()\n", value);
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
		printf("ERROR: ioctl error in write_to_driver_gpio() (ret: %d)\n", ret);
		return -1;
	}

	return 1;
}

int read_from_driver_gpio(int fd, int pin) {
	int ret;

	/*  Pass GPIO struct with IO control */
	memset(&apin , 0, sizeof(apin));
	strcpy(apin.desc, "Readpin");
	apin.pin = pin;

	/* Pass 'apin' struct to 'fd' with IO control*/
	ret = ioctl(fd, IOCTL_PIIO_GPIO_READ, &apin);
	printf("READ: Requested  pin:%i - val:%i - desc:%s\n" , apin.pin , apin.value, apin.desc);

	if (ret < 0) {
		printf("ERROR: ioctl error in read_from_driver_gpio() (ret: %d)\n", ret);
		return -1;
	}

	return apin.value;
}

int toggle_pin(int fd, int pin, int times, int ms) {
	int value = read_from_driver_gpio(fd, pin);

	if (value < 0 || value > 1) {
		printf("ERROR: value is greater than 0 or less than 1 (value: %i) in toggle_pin()\n", value);
		return -1;
	} 

	value = value == 1 ? 0 : 1;

	for (int i = 0; i < times; i++) {		
		printf("TOGGLE: pin:%i - times:%i - current:%i - value:%i - ms:%i\n", pin, times, i, value, ms);		
		write_to_driver_gpio(fd, pin, value);
		usleep(ms * 1000);
		value = value == 1 ? 0 : 1;
	}

	return 1;
}

void show_menu(int argc, char *argv[], int fd) {
	if (argc > 1) {
		if (!strncmp(argv[1], "readmsg", 8)) {
			read_from_driver(fd);
		}

		else if (!strncmp(argv[1], "writemsg", 9)) {
			write_to_driver(fd);
		}

		else if (!strncmp(argv[1], "readpin", 8)) {
			if (argc < 3) {
				printf("USAGE: piio readpin [pin]\n");
				return;
			}

			int pin = strtol(argv[2], NULL, 10);
			read_from_driver_gpio(fd, pin);
		}

		else if (!strncmp(argv[1], "writepin", 9)) {
			if (argc < 4) {
				printf("USAGE: piio writepin [pin] [value(1/0)]\n");
				return;
			}

			int pin = strtol(argv[2], NULL, 10);
			int value = strtol(argv[3], NULL, 10);
			write_to_driver_gpio(fd, pin, value);
		}

		else if (!strncmp(argv[1], "toggle", 7)) {
			if (argc < 5) {
				printf("USAGE: piio toggle [pin] [times] [ms]\n");
				return;
			}

			int pin = strtol(argv[2], NULL, 10);
			int times = strtol(argv[3], NULL, 10);
			int ms = strtol(argv[4], NULL, 10);
			toggle_pin(fd, pin, times, ms);
		}
	} else {
		printf("USAGE: piio [readmsg/writemsg/readpin/writepin/toggle]\n");
	}
}

int main(int argc, char *argv[]) {
	printf("User App\n");
	int fd, ret;
	char *msg = "Message passed by ioctl\n";
	char drvPath[30];
	
	sprintf(drvPath, "//dev//%s", DEVICE_NAME);

	fd = open(drvPath, O_RDWR);
	if (fd < 0) {
		printf("ERROR: Can't open device file: %s\n", DEVICE_NAME);
		exit(-1);
	}

	show_menu(argc, argv, fd);

	printf("Exit 0\n");

	close(fd);
	return 0;
}