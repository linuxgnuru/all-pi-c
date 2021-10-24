/* Header file associated with SPI called PCD8544 devices etc.
 * Adapted from ../PCD8544_RPi_BBSPI/PCD8544.h by F.T.Gowen ...
 * PCD8544.h Copyright (C) 2010 Limor Fried, Adafruit Industries
 * CORTEX-M3 version by Le Dang Dung, 2011 LeeDangDung@gmail.com (tested on LPC1769)
 * Raspberry Pi version by Andre Wussow, 2012, desk@binerry.de
 * 
 * Description : Modified (ie. not bit-banged SPI) PCD8544 LCD library!
 * 
 * ================================================================================
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * ================================================================================
 * 
 * $Id: spi_PCD8544.h,v 1.3 2018/03/31 21:31:30 pi Exp $
 */

/* Conditionally Flag this file */
#ifndef SPI_PCD8544_H
#define SPI_PCD8544_H 1

#ifdef NO_MAIN
#ifndef _STDINT_H
#include <stdint.h>
#endif /* !_STDINT_H */
#ifndef _CTYPE_H
#include <ctype.h>
#endif /* !_CTYPE_H */
/* Check for config.h etc. ... */
#ifdef HAVE_CONFIG_H
#include <config.h>
#ifdef HAVE_WIRINGPI_H
#include <wiringPi.h>
#endif /* HAVE_WIRINGPI_H */
#ifdef HAVE_WIRINGPISPI_H
#include <wiringPiSPI.h>
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_CONFIG_H */
#ifndef I2C_WRAP_H
#include "i2c_wraps.h"
#endif /* !I2C_WRAP_H */
#ifndef SPI_CHIP_H
#include "spi_chips.h"
#endif /* !SPI_CHIP_H */
#ifndef SPI_WRAP_H
#include "spi_wraps.h"
#endif /* !SPI_WRAP_H */
#ifndef GLCD_XBMTOOLS_H
#include "glcd_xbmtools.h"
#endif /* !GLCD_XBMTOOLS_H */
#endif /* NO_MAIN */

#define BLACK 1
#define WHITE 0

#define LCDWIDTH  84
#define LCDHEIGHT 48

#define PCD8544_POWERDOWN           0x04
#define PCD8544_ENTRYMODE           0x02
#define PCD8544_EXTENDEDINSTRUCTION 0x01

#define PCD8544_DISPLAYBLANK    0x00
#define PCD8544_DISPLAYNORMAL   0x04
#define PCD8544_DISPLAYALLON    0x01
#define PCD8544_DISPLAYINVERTED 0x05

/* H = 0 */
#define PCD8544_FUNCTIONSET    0x20
#define PCD8544_DISPLAYCONTROL 0x08
#define PCD8544_SETYADDR       0x40
#define PCD8544_SETXADDR       0x80

/* H = 1 */
#define PCD8544_SETTEMP 0x04
#define PCD8544_SETBIAS 0x10
#define PCD8544_SETVOP  0x80

/* keywords */
#define LSBFIRST  0
#define MSBFIRST  1

/* BackLight constants */
#define PCD8544_BL_NONE      -1 /* IE. No backlight Control */
#define PCD8544_BL_CC_NPN     1 /* Common Cathode or NPN transistor drive */
#define PCD8544_BL_CC_NPN_ON  1
#define PCD8544_BL_CC_NPN_OFF 0
#define PCD8544_BL_CA_PNP     0 /* Common Anode or PNP transistor drive */
#define PCD8544_BL_CA_PNP_ON  0
#define PCD8544_BL_CA_PNP_OFF 1

/* Ensure globals only declared once ... */
/* Relies on NO_MAIN being defined in libraries */
/* NB: unsigned char is == to arduino's uint8_t */
#ifndef NO_MAIN
/* LCD port variables (from PCD8544.c) */
unsigned char cursor_x, cursor_y, textsize, textcolor;
/* The memory buffer for the LCD (adapted PCD8544.c) */
unsigned char pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8];
#else /* !NO_MAIN */
/* LCD port variables (from PCD8544.c) */
extern unsigned char cursor_x, cursor_y, textsize, textcolor;
/* The memory buffer for the LCD (adapted PCD8544.c) */
extern unsigned char pcd8544_buffer[LCDWIDTH * LCDHEIGHT / 8];
#endif /* !NO_MAIN */

/* Prototypes */

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
/* Full SPI routines originally in spi_glcd_demo.c */
void WP_FS_WriteCommandByte(pcd8544 *this_pcd8544,
			    char qx);

void WP_FS_WriteDataByte(pcd8544 *this_pcd8544,
			 char qx);

void WP_FS_ClearHome(pcd8544 *this_pcd8544, char qx);

void WP_FS_WriteFontDataBytes(pcd8544 *this_pcd8544,
			      unsigned int ascii_code,
			      char *this8x5font,
			      char qx);

void WP_FS_WriteGraphicsDataBytes(pcd8544 *this_pcd8544,
				  pcd8544_colpix8 *this_colpix8,
				  char qx);
void WP_FS_WriteMessage(pcd8544 *this_pcd8544,
			char *this8x5font,
			char qx,
			char *message);
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */
#endif /* !SPI_PCD8544_H */

/* Change Log */
/*
 * $Log: spi_PCD8544.h,v $
 * Revision 1.3  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.2  2017/08/19 18:05:15  pi
 * Implicit definition warnings solved and code cleaned up
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.5  2015/01/25 13:01:17  pi
 * Function moved from spi_glcdV2-demo into here
 *
 * Revision 1.4  2015/01/16 23:28:17  pi
 * Interim commit
 *
 * Revision 1.3  2015/01/15 16:40:21  pi
 * Interim commit
 *
 * Revision 1.2  2015/01/15 14:03:22  pi
 * Beginning to move or create Full Spi generic functions
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.3  2014/06/21 23:01:23  pi
 * Interim commit
 *
 * Revision 1.2  2014/06/21 18:07:24  pi
 * Interim commit
 *
 * Revision 1.1  2014/06/21 18:04:16  pi
 * Initial revision
 *
 *
 *
 */
