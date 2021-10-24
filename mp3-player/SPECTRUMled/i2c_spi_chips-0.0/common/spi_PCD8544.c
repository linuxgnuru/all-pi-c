/* C source file associated with SPI called PCD8544 devices etc.
 * $Id: spi_PCD8544.c,v 1.2 2018/03/31 21:31:30 pi Exp $
 * Adapted from ../PCD8544_RPi_BBSPI/PCD8544.c by F.T.Gowen ...
 * PCD8544.c copyright (C) 2010 Limor Fried, Adafruit Industries
 * CORTEX-M3 version by Le Dang Dung, 2011 LeeDangDung@gmail.com (tested on LPC1769)
 * Raspberry Pi version by Andre Wussow, 2012, desk@binerry.de
 * 
 * Description :
 *     A simple PCD8544 LCD (Nokia3310/5110) driver. Target board is Raspberry Pi.
 * 
 *  References  :
 *  http://www.arduino.cc/playground/Code/PCD8544
 *  http://ladyada.net/products/nokia5110/
 *  http://code.google.com/p/meshphone/
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
 */

#define NO_MAIN
#include "spi_PCD8544.h"
#ifndef PCD8544_FONT_H
#include "PCD8544_font.h"
#endif /* !PCD8544_FONT_H */

/* bit set (from PCD8544.c) */
#define _BV(bit) (0x1 << (bit))

#ifdef HAVE_WIRINGPI_H
#ifdef HAVE_WIRINGPISPI_H
/* Full SPI routines originally in spi_glcd_demo.c */
void WP_FS_WriteCommandByte(pcd8544 *this_pcd8544,
			    char qx)
{
  int ldc_gpio;
  unsigned char ldcval;
  ldc_gpio  = (*this_pcd8544).dc_gpio;
  ldcval    = (*this_pcd8544).dcval;
  /* Data/¬Command = Command mode */
  WP_digitalWrite(ldc_gpio, GPIO_LOW, qx);
  delayMicroseconds(PCD8544_VFAST_CLOCK);
  WP_write_byte(*(*this_pcd8544).spifd, ldcval, qx);
  delayMicroseconds(PCD8544_VFAST_CLOCK);
}

void WP_FS_WriteDataByte(pcd8544 *this_pcd8544,
			 char qx)
{
  int ldc_gpio;
  unsigned char ldcval;
  ldc_gpio  = (*this_pcd8544).dc_gpio;
  ldcval    = (*this_pcd8544).dcval;
  /* Data/¬Command = Data mode */
  WP_digitalWrite(ldc_gpio, GPIO_HIGH, qx);
  delayMicroseconds(PCD8544_VFAST_CLOCK);
  WP_write_byte(*(*this_pcd8544).spifd, ldcval, qx);
  delayMicroseconds(PCD8544_VFAST_CLOCK);
}

void WP_FS_ClearHome(pcd8544 *this_pcd8544, char qx)
{
  int l,m;
  m = 84 * 6;
  (*this_pcd8544).dcval = PCD8544_SETXADDR;
  WP_FS_WriteCommandByte(this_pcd8544, qx);
  (*this_pcd8544).dcval = PCD8544_SETYADDR;
  WP_FS_WriteCommandByte(this_pcd8544, qx);
  (*this_pcd8544).dcval = 0x00;
  for (l=0; l<m; l++) WP_FS_WriteDataByte(this_pcd8544, qx);
  (*this_pcd8544).dcval = PCD8544_SETXADDR;
  WP_FS_WriteCommandByte(this_pcd8544, qx);
  (*this_pcd8544).dcval = PCD8544_SETYADDR;
  WP_FS_WriteCommandByte(this_pcd8544, qx);
}

void WP_FS_WriteFontDataBytes(pcd8544 *this_pcd8544,
			      unsigned int ascii_code,
			      char *this8x5font,
			      char qx)
{
  unsigned char *colptr;
  char l;
  colptr = this8x5font + (ascii_code * 5);
  for (l=0; l<5; l++)
    {
      (*this_pcd8544).dcval = *colptr;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      colptr++;
    }
  (*this_pcd8544).dcval = 0x00;
  WP_FS_WriteDataByte(this_pcd8544, qx);
}

void WP_FS_WriteGraphicsDataBytes(pcd8544 *this_pcd8544,
				  pcd8544_colpix8 *this_colpix8,
				  char qx)
{  
  unsigned char* ucp;
  int i;
  ucp = (*this_colpix8).rowblock0;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_pcd8544).dcval = *ucp;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock1;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_pcd8544).dcval = *ucp;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock2;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_pcd8544).dcval = *ucp;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock3;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_pcd8544).dcval = *ucp;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock4;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_pcd8544).dcval = *ucp;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      ucp++;
    }
  ucp = (*this_colpix8).rowblock5;
  for (i=0; i<PCD8544_row_height; i++)
    {
      (*this_pcd8544).dcval = *ucp;
      WP_FS_WriteDataByte(this_pcd8544, qx);
      ucp++;
    }
}

/* Full SPI routine originally in spi_glcdV2_demo.c */
void WP_FS_WriteMessage(pcd8544 *this_pcd8544,
			char *this8x5font,
			char qx,
			char *message)
{
  int bite;
  char lmsg[LCDWIDTH+1],*lp1;
  strncpy(lmsg, message, LCDWIDTH);
  lmsg[LCDWIDTH] = NUL;
  lp1 = lmsg;
  while (*lp1 != NUL)
    {
      bite = toascii((int)(*lp1));
      WP_FS_WriteFontDataBytes(this_pcd8544, bite, this8x5font, qx);
      lp1++;
    }
}			
#endif /* HAVE_WIRINGPISPI_H */
#endif /* HAVE_WIRINGPI_H */

/* Change Log */
/*
 * $Log: spi_PCD8544.c,v $
 * Revision 1.2  2018/03/31 21:31:30  pi
 * Unison synch
 *
 * Revision 1.1  2018/03/08 20:44:31  pi
 * Initial revision
 *
 * Revision 1.1  2015/11/04 22:28:03  pi
 * Initial revision
 *
 * Revision 1.4  2015/01/25 13:01:17  pi
 * Function moved from spi_glcdV2-demo into here
 *
 * Revision 1.3  2015/01/16 23:28:17  pi
 * Interim commit
 *
 * Revision 1.2  2015/01/15 14:03:22  pi
 * Beginning to move or create Full Spi generic functions
 *
 * Revision 1.1  2014/07/18 19:12:11  pi
 * Initial revision
 *
 * Revision 1.2  2014/06/21 23:01:23  pi
 * Interim commit
 *
 * Revision 1.1  2014/06/21 18:04:16  pi
 * Initial revision
 *
 *
 *
 */
