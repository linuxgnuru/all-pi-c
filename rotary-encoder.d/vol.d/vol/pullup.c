#include <stdio.h>

#include <wiringPi.h>

#define TRUE 1
#define FALSE 0

const int pausePin = 22;
const int  prevPin = 21;
const int  nextPin = 5;
const int  quitPin = 2;
const int  infoPin = 1;

int main()
{
	int myFlag;

	myFlag = TRUE;
	wiringPiSetup();
	pinMode(pausePin, INPUT);
	pullUpDnControl(pausePin, PUD_UP);
	pinMode(prevPin, INPUT);
	pullUpDnControl(prevPin, PUD_UP);
	pinMode(nextPin, INPUT);
	pullUpDnControl(nextPin, PUD_UP);
	pinMode(quitPin, INPUT);
	pullUpDnControl(quitPin, PUD_UP);
	pinMode(infoPin, INPUT);
	pullUpDnControl(infoPin, PUD_UP);
	while (myFlag == TRUE)
	{
		if (digitalRead(pausePin) == 0) printf("pause\n");
		if (digitalRead(prevPin) == 0) printf("prev\n");
		if (digitalRead(nextPin) == 0) printf("next\n");
		if (digitalRead(infoPin) == 0) printf("info\n");
		if (digitalRead(quitPin) == 0)
		{
			printf("quit\n");
			myFlag = FALSE;
		}
		//delay(500);
	}
	/*
	pinMode(G_PIN, INPUT);
	pullUpDnControl(G_PIN, PUD_UP);
	pinMode(G_POUT, OUTPUT);
	for (;;)
		digitalWrite(G_POUT, (digitalRead(G_PIN) == 0 ? HIGH : LOW));
	*/
	return 0;
}
