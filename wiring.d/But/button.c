#include <stdio.h>

#include <wiringPi.h>

#define TRUE 1
#define FALSE 0

const int pausePin = 22;
const int  prevPin = 21;
const int  nextPin = 5;
const int  quitPin = 2;
const int  infoPin = 1;

void pauseButton()
{
	digitalWrite(25, HIGH);
	delay(800);
	digitalWrite(25, LOW);
}

void prevButton()
{
	digitalWrite(25, HIGH);
	delay(400);
	digitalWrite(25, LOW);
	digitalWrite(25, HIGH);
	digitalWrite(25, LOW);
}

void nextButton()
{
	int i;
	for (i = 0; i < 5; i++)
	{
		digitalWrite(25, HIGH);
		delay(600);
		digitalWrite(25, LOW);
	}
}

void quitButton()
{
	int i;
	for (i = 0; i < 10; i++)
	{
		digitalWrite(25, HIGH);
		delay(1000);
		digitalWrite(25, LOW);
	}
}

void infoButton()
{
	digitalWrite(25, HIGH);
	delay(1000);
	digitalWrite(25, LOW);
	digitalWrite(25, HIGH);
	delay(1000);
	digitalWrite(25, LOW);
}

int main()
{
	int ctr;

	ctr = 0;
	wiringPiSetup();
	pinMode(25, OUTPUT);
	wiringPiISR(pausePin, INT_EDGE_RISING, &pauseButton);
	wiringPiISR(prevPin,  INT_EDGE_RISING, &prevButton);
	wiringPiISR(nextPin,  INT_EDGE_RISING, &nextButton);
	wiringPiISR(quitPin,  INT_EDGE_RISING, &quitButton);
	wiringPiISR(infoPin,  INT_EDGE_RISING, &infoButton);
	//wiringPiISR(G_PIN,  INT_EDGE_RISING, &pushButton);
	while (1)
	{
		ctr++;
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
