/*
   +-----+------+---------+----+-----+---+----+----+----+----+----+----+----+----+------+-----+
   | VSS | VDD  |    V0   | RS | RW  | E | D4 | D5 | D6 | D7 | D4 | D5 | D6 | D7 |   A  |  K  |
   +-----+------+---------+----+-----+---+----+----+----+----+----+----+----+----+------+-----+
   | GND | 3.3v | 10k pot |    | GND |   | NC | NC | NC | NC |    |    |    |    | 3.3v | GND |
   +-----+------+---------+----+-----+---+----+----+----+----+----+----+----+----+------+-----+
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <lcd.h>

#define RS 24
#define EN 27
#define D4 6
#define D5 5
#define D6 4
#define D7 3

int lcdFD;

// catch signals and clean up GPIO
static void die(int sig)
{
    lcdClear(lcdFD);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

int main(int argc, char *argv[])
{
    char msg1[16];
    char msg2[16];
    int len;

    // first make sure there is something to type
    if (argc <= 1)
    {
        //fprintf (stderr, "Usage: %s\n", strerror (errno)) ;
        printf ("Usage:\n %s [ LINE1 ] ([ LINE2 ])\nMaximum 16 characters and LINE2 is optional\n", argv[0]);
        return 1;
    }
    // test for string lengths
    len = strlen(argv[1]);
    if (len > 16)
    {
        printf("Length of message is too long.\n");
        return 1;
    }
    else if ((argc == 3) && (strlen(argv[2]) > 16))
    {
        printf("Length of second message is too long.\n");
        return 1;
    }
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    // try to connect to display
    if (wiringPiSetup() < 0)
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }
    lcdFD = lcdInit(2, 16, 4, RS, EN, D4, D5, D6, D7, D4, D5, D6, D7);
    strncpy(msg1, argv[1], 16);
    lcdPosition(lcdFD, 0,0);
    lcdPrintf(lcdFD, msg1);
    if (argc == 3)
    {
        strcpy(msg2, argv[2]);
        lcdPosition(lcdFD, 0, 1);
        lcdPrintf(lcdFD, msg2);
    }
    return EXIT_SUCCESS;
}

