/* Header file associated with PIGPIO WRAPperS etc.
 * $Id: pigpio_wraps.h,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef PIGPIO_WRAP_H
#define PIGPIO_WRAP_H 1

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
#ifdef HAVE_PIGPIO_H
#include <pigpio.h>
#endif /* HAVE_PIGPIO_H */
#endif /* HAVE_CONFIG_H */
#endif /* NO_MAIN */

#ifdef IS_MAIN
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_PIGPIO_H
#include <pigpio.h>
#endif /* HAVE_PIGPIO_H */
#endif /* HAVE_CONFIG_H */
#endif /* NO_MAIN */

#ifndef PI_B1
#define PI_B1 0
#endif /* PI_B1 */
#ifndef PI_B2
#define PI_B2 1
#endif /* PI_B2 */

#ifdef HAVE_PIGPIO_H

#define PG_BLOCK_SIZE    32
#define PG_UNDEF_HANDLE 999

typedef struct
{
  unsigned int i2cBus;
  unsigned int i2cAddr;
  unsigned int i2cFlags; /* Currently always set to zero - see pigpio.h */
  unsigned int handle;
  unsigned int i2cReg;
  unsigned int bVal;
  unsigned int wVal;
  char block_buf[PG_BLOCK_SIZE];
} pgi2cvb; /* PiGpio I2C Variables Block */

void PG_i2cset(pgi2cvb pgi2c,
	       char qx);
  
void PG_i2cset2(pgi2cvb pgi2c,
		char qx);

void PG_i2cseti(pgi2cvb pgi2c,
		unsigned int count,
		char qx);

void PG_i2cout(pgi2cvb pgi2c,
	       char qx);

#endif /* HAVE_PIGPIO_H */

#endif /* !PIGPIO_WRAP_H */

/* Change Log */
/*
 * $Log: pigpio_wraps.h,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.3  2017/12/21 20:02:15  pi
 * Interim commit
 *
 * Revision 1.2  2017/08/19 18:05:15  pi
 * Implicit definition warnings solved and code cleaned up
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.4  2015/04/27 21:21:09  pi
 * Interim commit
 *
 * Revision 1.3  2015/04/26 19:10:56  pi
 * Interim commit
 *
 * Revision 1.2  2015/04/25 16:47:30  pi
 * Interim commit
 *
 * Revision 1.1  2015/04/25 11:53:40  pi
 * Initial revision
 *
 *
 *
 */
