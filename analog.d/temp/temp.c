#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <wiringPi.h>

#define beta 4090 //the beta of the thermistor

#define MY_8591 100

const int addr = 0x48;

static void die(int sig)
{
  if (sig != 0 && sig != 2)
      (void)fprintf(stderr, "caught signal %d\n", sig);
  if (sig == 2)
      (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
  exit(0);
}

float getTemp(long val, int cf)
{
  //float tempC = beta /(log((1025.0 * 10 / val - 10) / 10) + beta / 298.0) - 273.0;
  float tempC = beta /(logf((257.0 * 10 / val - 10) / 10) + beta / 298.0) - 273.0;
  float tempF = ((tempC * 9) / 5) + 32;
  return (cf == 1 ? tempC : tempF);
}

int main(int argc, char **argv)
{
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
    printf("%f\n", getTemp(analogRead(MY_8591 + 0), 2));
    return EXIT_SUCCESS;
}
#if 0
	if (argc > 1)
	{
		num_sensors = atoi(argv[1]);
		if (num_sensors < 1 || num_sensors > 4)
		{
			printf("Error: number must be between 1 and 4\n");
			return(-1);
		}
	}
	wiringPiSetup();
	pcf8591Setup(MY_8591, addr);
	initscr();
	noecho();
	cbreak();
	nodelay(stdscr, true);
	curs_set(0);
	printw("PCF8591");
	mvaddstr(1, 0, "Analog output");
	mvaddstr(2, 0, "0x48");
	refresh();
	while (1)
	{
		for (i = 0; i < num_sensors; i++)
		{
			// the read is always one step behind the selected input
			move(3 + i, 0);
			printw("Input: %d Data: %0.3d", i, abs(analogRead(MY_8591 + i) - 255));
			refresh();
			delay(delay_n);
		}
		key = getch();
		if (key > -1)
			break;
	}
	endwin();
	return(0);
}
#endif
