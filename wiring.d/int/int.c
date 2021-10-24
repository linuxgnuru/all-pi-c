#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <ncurses.h>

#include <wiringPi.h>

static volatile int globalCounter;

void myInterrupt(void) { ++globalCounter; }

int main(void)
{
  int gotOne;
  int myCounter;

  initscr();
  noecho();
  cbreak();
  nodelay(stdscr, true);
  globalCounter = myCounter = 0;
  wiringPiSetup();
  wiringPiISR(3, INT_EDGE_RISING, &myInterrupt);
  curs_set(0);
  printw("Waiting...");
  refresh();
  for (;;)
  {
    gotOne = 0;
    fflush(stdout);
    for (;;)
    {
	if (globalCounter != myCounter)
	{
	  move(2, 0);
	  printw(" Int on pin 3: Counter: %5d", globalCounter);
	  refresh();
	  myCounter = globalCounter;
	  ++gotOne;
	}
      	if (gotOne != 0)
		break;
    }
  }
  endwin();
  return 0;
}
