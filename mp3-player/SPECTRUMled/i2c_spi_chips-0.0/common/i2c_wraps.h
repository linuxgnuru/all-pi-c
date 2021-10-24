/* Header file associated with I2C WRAPperS etc.
 * $Id: i2c_wraps.h,v 1.2 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef I2C_WRAP_H
#define I2C_WRAP_H 1

#ifdef NO_MAIN
#define TRUE       1
#define FALSE      0
#define NUL        '\0'
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPII2C_H
#include <wiringPiI2C.h>
#endif /* HAVE_WIRINGPII2C_H */

/*
#ifdef HAVE_PIGPIO_H
#include <pigpio.h>
#endif /* HAVE_PIGPIO_H */

#endif /* HAVE_CONFIG_H */
#endif /* NO_MAIN */

#ifndef CMD_LEN
#define CMD_LEN         138  /* 2 x FN_LEN + 10   */
#endif /* CMD_LEN */

#define PI_B1 0
#define PI_B2 1

void C2S_i2cdetect(int chan,
		   char qx);

void C2S_i2cset(int chan,
		unsigned int addr,
		unsigned int regr,
		unsigned int data,
		char qx);

void C2S_i2cseti(int chan,
		 unsigned int addr,
		 unsigned int regr,
		 unsigned int data[16],
		 unsigned int nval,
		 char qx);

void C2S_i2cout(int chan,
		unsigned int addr,
		unsigned int data,
		char qx);

#ifdef HAVE_WIRINGPI_H
void C2S_gpio(char *options,
	      char qx);

#define GPIO_XIN  0
#define GPIO_XOUT 1

void C2S_gpio_export(int gpio_pin,
		     int gpio_mode,
		     char qx);

void C2S_gpio_unexport(int gpio_pin,
		       char qx);

void C2S_gpio_unexportall(char qx);

#define GPIO_MODE_IN  -1
#define GPIO_MODE_OUT  1

void C2S_gpio_g_mode(int gpio_gpin,
		     int gpio_mode,
		     char qx);

void C2S_gpio_g_write(int gpio_gpin,
		     int gpio_level,
		     char qx);

void C2S_gpio_mode(int gpio_wpin,
		   int gpio_mode,
		   char qx);

void C2S_gpio_write(int gpio_wpin,
		    int gpio_level,
		    char qx);

void C2S_gpio_wb(unsigned int gpio_byte,
		 char qx);

#define GPIO_HIGH HIGH
#define GPIO_LOW  LOW

void WP_pinMode(int wpin,
		int mode,
		char qx);

void WP_digitalWrite(int gpio_pin,
		     int value,
		     char qx);

void WP_digitalWriteByte(int value,
			 char qx);
#endif /* HAVE_WIRINGPI_H */

#ifdef HAVE_WIRINGPII2C_H

#define WP_MAX_IBYTES 128

typedef struct
{
  int did;
  int fd;
  int reg;
  int data;
  char ibytes[WP_MAX_IBYTES];
} wpi2cvb; /* WiringPiI2C Variables Block */

void WP_i2cset(wpi2cvb wpi2c,
	       char qx);

void WP_i2cset2(wpi2cvb wpi2c,
		char qx);

void WP_i2cseti(wpi2cvb wpi2c,
		int nbytes,
		char qx);

void WP_i2cout(wpi2cvb wpi2c,
	       char qx);

int WP_i2cget(wpi2cvb wpi2c,
	      char qx);

#endif /* HAVE_WIRINGPII2C_H */

/*
#ifdef HAVE_PIGPIO_H

#define PG_BLOCK_SIZE 32

typedef struct
{
  unsigned int i2cBus;
  unsigned int i2cAddr;
  unsigned int i2cFlags;
  unsigned int handle;
  unsigned int i2cReg;
  unsigned int bVal;
  unsigned int wVal;
  char block_buf[PG_BLOCK_SIZE];
} pgi2cvb;

void PG_i2cset(pgi2cvb pgi2c,
	       char qx);
  
void PG_i2cset2(pgi2cvb pgi2c,
		char qx);

#endif /* HAVE_PIGPIO_H */

#endif /* !I2C_WRAP_H */

/* Change Log */
/*
 * $Log: i2c_wraps.h,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.8  2015/04/25 11:53:40  pi
 * Moving code to pigpio_wraps.h
 *
 * Revision 1.7  2015/04/24 14:07:52  pi
 * Interim commit
 *
 * Revision 1.6  2015/04/23 22:07:36  pi
 * Beginning to use pigpio methods
 *
 * Revision 1.5  2014/12/22 17:00:42  pi
 * Interim commit
 *
 * Revision 1.4  2014/12/21 20:05:18  pi
 * Beginning to add bit-banged spi support for nokia glcd
 *
 * Revision 1.3  2014/12/05 19:14:13  pi
 * Interim commit
 *
 * Revision 1.2  2014/12/04 23:00:48  pi
 * C2S_gpio_wb function added
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.1  2014/05/26 21:44:15  pi
 * Initial revision
 *
 * Revision 1.7  2014/03/21 21:48:18  pi
 * More gpio system call wrappers added etc.
 *
 * Revision 1.6  2014/03/02 22:47:02  pi
 * Interim commit
 *
 * Revision 1.5  2014/03/02 15:12:56  pi
 * Interim commit
 *
 * Revision 1.4  2014/03/01 18:21:13  pi
 * New routines etc. for LCD backpack device added
 *
 * Revision 1.3  2013/06/11 20:50:52  pi
 * Interim commit
 *
 * Revision 1.2  2013/06/07 21:28:53  pi
 * Interim commit
 *
 * Revision 1.1  2013/06/03 20:58:09  pi
 * Initial revision
 *
 * Revision 1.6  2013/05/26 23:32:06  pi
 * Added WP_i2cget()
 *
 * Revision 1.5  2013/05/26 21:51:20  pi
 * Added C2S_i2cout()
 *
 * Revision 1.4  2013/05/26 21:23:45  pi
 * WP_i2cout() added
 *
 * Revision 1.3  2013/05/15 23:07:35  pi
 * Interim commit
 *
 * Revision 1.2  2013/05/15 22:30:47  pi
 * Interim commit
 *
 * Revision 1.1  2013/05/15 22:10:05  pi
 * Initial revision
 *
 *
 *
 */
