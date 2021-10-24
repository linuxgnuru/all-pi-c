#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>
#include <signal.h>
#include <errno.h>

#include <wiringPi.h>

#define MY_8574 100

const addr = 0x20;

void sig_handler(int signo)
{
	int i;

	if (signo == SIGINT || signo == SIGTERM)
	{
		//printf("SIGINT recieved\n");
		printf("\nGood bye.\n");
		exit(1);
	}
}

int main(int argc, char **argv)
{
	int i;

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		printf("\n Can't catch SIGINT. \n");
    if (wiringPiSetup() == -1)
    {
	    fprintf(stdout, "wiringPiSetup failure: %s\n", strerror(errno));
	    return EXIT_FAILURE;
    }
	pcf8574Setup(MY_8574, addr);
    printf("HIGH\n");
    for (i = 0; i < 8; i++)
    {
        digitalWrite(MY_8574 + i, HIGH);
        delay(500);
    }
    printf("LOW\n");
    for (i = 0; i < 8; i++)
    {
        digitalWrite(MY_8574 + i, LOW);
        delay(500);
    }
	return 0;
}

