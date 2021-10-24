/* Header file associated with PIGPIOD WRAPperS etc.
 * $Id: pigpiod_wraps.h,v 1.4 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef PIGPIOD_WRAP_H
#define PIGPIOD_WRAP_H 1

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
#ifdef HAVE_PIGPIOD_IF2_H
#include <pigpiod_if2.h>
#endif /* HAVE_PIGPIOD_IF2_H */
#endif /* HAVE_CONFIG_H */
#endif /* NO_MAIN */

#ifdef IS_MAIN
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_PIGPIOD_IF2_H
#include <pigpiod_if2.h>
#endif /* HAVE_PIGPIOD_IF2_H */
#endif /* HAVE_CONFIG_H */
#endif /* NO_MAIN */

#ifndef PI_B1
#define PI_B1 0
#endif /* PI_B1 */
#ifndef PI_B2
#define PI_B2 1
#endif /* PI_B2 */

#ifdef HAVE_PIGPIOD_IF2_H

/*
#define PG_BLOCK_SIZE    32
*/
#define PGD_UNDEF_HANDLE 999

typedef struct
{
  int thispi;
  unsigned int i2cbus;
  unsigned int i2caddr;
  unsigned int i2cflags; /* Currently always set to zero - see pigpiod_if2.h */
  unsigned int handle;
  unsigned int i2cReg;
  unsigned int bVal;
  unsigned int wVal;
  /*  char block_buf[PG_BLOCK_SIZE]; */
} pgdi2cvb; /* PiGpioD I2C Variables Block */


void PGD_i2cset(pgdi2cvb pgdi2c,
		char qx);

void PGD_i2cset2(pgdi2cvb pgdi2c,
		 char qx);

/*
void PGD_i2cseti(pgdi2cvb pgdi2c,
		 unsigned int count,
		 char qx);
*/

void PGD_i2cout(pgdi2cvb pgdi2c,
		char qx);

#endif /* HAVE_PIGPIOD_IF2_H */

#endif /* !PIGPIO_WRAP_H */

/* Change Log */
/*
 * $Log: pigpiod_wraps.h,v $
 * Revision 1.4  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.3  2017/12/22 23:32:51  pi
 * Interim Commit
 *
 * Revision 1.2  2017/12/21 20:02:15  pi
 * Interim commit
 *
 * Revision 1.1  2017/12/21 19:07:13  pi
 * Initial revision
 *
 *
 *
 *
 */
