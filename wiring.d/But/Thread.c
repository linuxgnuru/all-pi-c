#include <stdio.h>

#include <wiringPi.h>

#define G_PIN 25
#define G_POUT 21

#define TRUE 1
#define FALSE 0

int ctr;

const int pausePin = 22;
const int  prevPin = 21;
const int  nextPin = 5;
const int  quitPin = 2;
const int  infoPin = 1;

/*
PI_THREAD (songThread)
{
	if (digitalRead(playButtonPin) == 0)
	{
		if (cur_song.play_status == PAUSE) playMe();
		else pauseMe();
	}
	delay(500);
}
*/

void pushButton()
{
	// toggle LED
	digitalWrite(G_POUT, (digitalRead(G_POUT) == HIGH ? LOW : HIGH));
	delay(500);
}

int main()
{
	int myFlag;
//	int t_ret;

	myFlag = TRUE;
	wiringPiSetup();
	pinMode(playButtonPin, INPUT);
	pullUpDnControl(playButtonPin, PUD_UP);
//	t_ret = piThreadCreate(songThread);
//	wiringPiISR(G_PIN, INT_EDGE_FALLING, &pushButton);
	//wiringPiISR(G_PIN, INT_EDGE_RISING, &pushButton);
	while (myFlag == TRUE)
	{
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
