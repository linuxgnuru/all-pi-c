/*
 * isr.c:
 *	Wait for Interrupt test program - ISR method
 *
 *	How to test:
 *	  Use the SoC's pull-up and pull down resistors that are avalable
 *	on input pins. So compile & run this program (via sudo), then
 *	in another terminal:
 *		gpio mode 0 up
 *		gpio mode 0 down
 *	at which point it should trigger an interrupt. Toggle the pin
 *	up/down to generate more interrupts to test.
 *
 * Copyright (c) 2013 Gordon Henderson.
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
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>

// globalCounter:
//	Global variable to count interrupts
//	Should be declared volatile to make sure the compiler doesn't cache it.

static volatile int globalCounter;

/*
 * myInterrupt:
 *********************************************************************************
 */

void myInterrupt3(void) { ++globalCounter; }

/*
 *********************************************************************************
 * main
 *********************************************************************************
 */

int main(void)
{
  int gotOne;
  int myCounter;

  globalCounter = myCounter = 0;
  wiringPiSetup();
  wiringPiISR(3, INT_EDGE_RISING, &myInterrupt3);
  pinMode(21, OUTPUT);
  for (;;)
  {
    gotOne = 0;
    printf("Waiting...");
    fflush(stdout);
    for (;;)
    {
	if (globalCounter != myCounter)
	{
	  printf(" Int on pin 3: Counter: %5d\n", globalCounter);
	  myCounter = globalCounter;
	  digitalWrite(21, HIGH);
	  ++gotOne;
	}
	else
	{
		digitalWrite(21, LOW);
      	}
      	if (gotOne != 0)
		break;
    }
  }
  return 0;
}
