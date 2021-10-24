/*
 * lcd.c:
 *	Text-based LCD driver.
 *	This is designed to drive the parallel interface LCD drivers
 *	based in the Hitachi HD44780U controller and compatables.
 *
 *	This test program assumes the following:
 *
 *	For 4-bit interface:
 *		GPIO 4-7 is connected to display data pins 4-7.
 *		GPIO 11 is the RS pin.
 *		GPIO 10 is the Strobe/E pin.
 *
 * Copyright (c) 2012-2013 Gordon Henderson.
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <unistd.h>
#include <string.h>
#include <time.h>

#include <wiringPi.h>
#include <lcd.h>

const int RS = 3;	// 3
const int EN = 14;	// 
const int D0 = 4;	// 4
const int D1 = 12;	// 
const int D2 = 13;	// 
const int D3 = 6;	// 6

#ifndef	TRUE
#  define	TRUE	(1==1)
#  define	FALSE	(1==2)
#endif

static unsigned char newChar[8] = 
{
  0b11111,
  0b10001,
  0b10001,
  0b10101,
  0b11111,
  0b10001,
  0b10001,
  0b11111,
};

// Global lcd handle:

static int lcdHandle;

/* * usage: */

int usage(const char *progName)
{
  fprintf (stderr, "Usage: %s bits cols rows\n", progName);
  return EXIT_FAILURE;
}

/* * scrollMessage: */

static const char *message =
/*          11111111112
   12345678901234567890 */
  "                    "
/*          111111111122222222223333333333444444444455
   123456789012345678901234567890123456789012345678901 */
  "Wiring Pi by Gordon Henderson. HTTP://WIRINGPI.COM/"
/*          11111111112
   12345678901234567890 */
  "                    ";

void scrollMessage(int line, int width)
{
  char buf [32];
  static int position = 0;
  static int timer = 0;

  if (millis() < timer)
    return;
  timer = millis() + 200;
  strncpy(buf, &message[position], width);
  buf[width] = 0;
  lcdPosition(lcdHandle, 0, line);
  lcdPuts(lcdHandle, buf);
  if (++position == (strlen(message) - width))
    position = 0;
}

/* * The works */

int main (int argc, char *argv[])
{
  int i;
  int lcd;
  int rows = 2;
  int cols = 16;
  int bits = 4;
  char buf [32];

  wiringPiSetup ();
  lcdHandle = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
  if (lcdHandle < 0)
  {
    fprintf (stderr, "%s: lcdInit failed\n", argv [0]);
    return -1;
  }
  for (;;)
  {
    scrollMessage(0, cols);
  }
  return 0;
}
