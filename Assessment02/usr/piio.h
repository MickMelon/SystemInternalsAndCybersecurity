
/*
 *  chardev.h - the header file with the ioctl definitions.
 *
 *  The declarations here have to be in a header file, because
 *  they need to be known both to the kernel module
 *  (in chardev.c) and the process calling ioctl (ioctl.c)
 */

#ifndef PIIO_H
#define PIIO_H

#include <linux/ioctl.h>

typedef struct lkm_data {
	unsigned char data[256];
	unsigned long len;
	char type;
} lkm_data;

typedef struct gpio_pin {
	char desc[16];
	unsigned int pin;
	int value;
	char opt;
} gpio_pin;

#define CMD_MAGIC_NUMBER '7'

#define IOCTL_PIIO_READ 		_IOR(CMD_MAGIC_NUMBER, 1, lkm_data)
#define IOCTL_PIIO_WRITE		_IOW(CMD_MAGIC_NUMBER, 2, lkm_data)
#define IOCTL_PIIO_GPIO_READ	_IOR(CMD_MAGIC_NUMBER, 3, gpio_pin)
#define IOCTL_PIIO_GPIO_WRITE	_IOW(CMD_MAGIC_NUMBER, 4, gpio_pin)

#define  DEVICE_NAME "dMichaelMcMillan"
#define  CLASS_NAME  "piiocls"

#endif
