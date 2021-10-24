#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <math.h>

#include <ncurses.h>

#include <wiringPi.h>
#include <pcf8591.h>

#define MY_8591 200

int delayTime = 50;
int addr = 100;

int showText = 1;

int cf = 1;

// catch signals and clean up GPIO
static void die(int sig)
{
    if (sig != 0 && sig != 2)
        (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2)
        (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    endwin();
    exit(0);
}

double map(double x, double x0, double x1, double y0, double y1)
{
    double y = y0 + ((y1 - y0) * ((x - x0) / (x1 - x0)));
    return y;
}

double getTemp()
{
    int RawADC;
    double tmp_Temp;
    double Temp = 0.0;

    RawADC = analogRead(MY_8591 + 1);
    tmp_Temp = log(10000.0 * ((255.0 / RawADC - 1)));
    tmp_Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * tmp_Temp * tmp_Temp )) * tmp_Temp);
    tmp_Temp = tmp_Temp - 273.15;            // Convert Kelvin to Celcius
    if (cf == 1)
        tmp_Temp = (tmp_Temp * 9.0) / 5.0 + 32.0; // Convert Celcius to Fahrenheit
    Temp += tmp_Temp;
    // Get average
    Temp /= (double) 2;
    return Temp;
}

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        if (strcmp(argv[1], "-d") == 0)
            showText = 0;
    }
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    pcf8591Setup(MY_8591, 0x48);
    initscr();
    noecho();
    cbreak();
    nodelay(stdscr, true);
    curs_set(0);
    printw("Temperature");
    refresh();
    for (;;)
    {
        //rawTemp = map(analogRead(MY_8591), 0, 255, 20, 300);
        move(2, 0);
        printw("%.2f", getTemp());
        refresh();
    }
    return EXIT_SUCCESS;
}

