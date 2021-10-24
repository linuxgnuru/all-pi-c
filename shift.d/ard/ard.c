#include <stdio.h>
#include <wiringPi.h>

int latchPin = 22; // green
int clockPin = 23; // yellow
int dataPin = 21; // blue

char dataRED1;
char dataGREEN1;
char dataArrayRED1[10];
char dataArrayGREEN1[10];

char dataRED2;
char dataGREEN2;
char dataArrayRED2[10];
char dataArrayGREEN2[10];

char dataRED3;
char dataGREEN3;
char dataArrayRED3[10];
char dataArrayGREEN3[10];

void shiftOut(int myDataPin, int myClockPin, char myDataOut)
{
  int i = 0;
  int pinState;

  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);
  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);
  for (i = 7; i >= 0; i--)
  {
    digitalWrite(myClockPin, 0);
    pinState = (myDataOut & (1<<i) ? 1 : 0);
    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }
  digitalWrite(myClockPin, 0);
}

void blinkAll_2Bytes(int n, int d)
{
  int x;
  digitalWrite(latchPin, 0);
  shiftOut(dataPin, clockPin, 0);
  shiftOut(dataPin, clockPin, 0);
  digitalWrite(latchPin, 1);
  delay(200);
  for (x = 0; x < n; x++)
  {
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 255);
    shiftOut(dataPin, clockPin, 255);
    digitalWrite(latchPin, 1);
    delay(d);
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, 0);
    shiftOut(dataPin, clockPin, 0);
    digitalWrite(latchPin, 1);
    delay(d);
  }
}

int main(int argc, char **argv)
{
  int k, j, l;
  wiringPiSetup();
  pinMode(latchPin, OUTPUT);
  dataArrayRED1[0] = 0xFF; //11111111
  dataArrayRED1[1] = 0xFE; //11111110
  dataArrayRED1[2] = 0xFC; //11111100
  dataArrayRED1[3] = 0xF8; //11111000
  dataArrayRED1[4] = 0xF0; //11110000
  dataArrayRED1[5] = 0xE0; //11100000
  dataArrayRED1[6] = 0xC0; //11000000
  dataArrayRED1[7] = 0x80; //10000000
  dataArrayRED1[8] = 0x00; //00000000
  dataArrayRED1[9] = 0x55; //00000001
  dataArrayRED2[0] = 0xAA; //11100000
  dataArrayRED2[1] = 0x55; //00000000
  dataArrayRED2[2] = 0xAA; //10000000
  dataArrayRED2[3] = 0x55; //11000000
  dataArrayRED2[4] = 0xAA; //11100000
  dataArrayRED2[5] = 0x55; //11110000
  dataArrayRED2[6] = 0xAA; //11111000
  dataArrayRED2[7] = 0x55; //11111100
  dataArrayRED2[8] = 0xAA; //11111110
  dataArrayRED2[9] = 0x55; //11111111
  dataArrayRED3[0] = 0x01; //11111111
  dataArrayRED3[1] = 0x00; //11111110
  dataArrayRED3[2] = 0x01; //11111100
  dataArrayRED3[3] = 0x03; //11111000
  dataArrayRED3[4] = 0x07; //11110000
  dataArrayRED3[5] = 0x0F; //11100000
  dataArrayRED3[6] = 0x1F; //11000000
  dataArrayRED3[7] = 0x3F; //10000000
  dataArrayRED3[8] = 0x7F; //00000000
  dataArrayRED3[9] = 0xAA; //00000000
  dataArrayGREEN1[0] = 0xFF; //11111111
  dataArrayGREEN1[1] = 0x7F; //01111111
  dataArrayGREEN1[2] = 0x3F; //00111111
  dataArrayGREEN1[3] = 0x1F; //00011111
  dataArrayGREEN1[4] = 0x0F; //00001111
  dataArrayGREEN1[5] = 0x07; //00000111
  dataArrayGREEN1[6] = 0x03; //00000011
  dataArrayGREEN1[7] = 0x01; //00000001
  dataArrayGREEN1[8] = 0x00; //00000000
  dataArrayGREEN1[9] = 0x55; //10000000
  dataArrayGREEN2[0] = 0x55; //11111111
  dataArrayGREEN2[1] = 0xAA; //01111111
  dataArrayGREEN2[2] = 0x55; //00111111
  dataArrayGREEN2[3] = 0xAA; //00011111
  dataArrayGREEN2[4] = 0x55; //00001111
  dataArrayGREEN2[5] = 0xAA; //00000111
  dataArrayGREEN2[6] = 0x55; //00000011
  dataArrayGREEN2[7] = 0xAA; //00000001
  dataArrayGREEN2[8] = 0x55; //00000000
  dataArrayGREEN2[9] = 0xAA; //00000111
  dataArrayGREEN3[0] = 0x80; //11111111
  dataArrayGREEN3[1] = 0x00; //11111110
  dataArrayGREEN3[2] = 0x01; //11111100
  dataArrayGREEN3[3] = 0x03; //11111000
  dataArrayGREEN3[4] = 0x07; //11110000
  dataArrayGREEN3[5] = 0x0F; //11100000
  dataArrayGREEN3[6] = 0x1F; //11000000
  dataArrayGREEN3[7] = 0x3F; //10000000
  dataArrayGREEN3[8] = 0x7F; //00000000
  dataArrayGREEN3[9] = 0xAA;
  blinkAll_2Bytes(2, 500); 
  while (1)
  {
  for (k = 0; k < 10; k++)
  {
    dataRED2 = dataArrayRED2[k];
    dataGREEN2 = dataArrayGREEN2[k];
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, dataGREEN2);
    shiftOut(dataPin, clockPin, dataRED2);
    digitalWrite(latchPin, 1);
    delay(300);
  }
  for (j = 0; j < 10; j++)
  {
    dataRED1 = dataArrayRED1[j];
    dataGREEN1 = dataArrayGREEN1[j];
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, dataGREEN1);
    shiftOut(dataPin, clockPin, dataRED1);
    digitalWrite(latchPin, 1);
    delay(300);
  }
  for (l = 0; l < 10; l++)
  {
    dataRED3 = dataArrayRED3[l];
    dataGREEN3 = dataArrayGREEN3[l];
    digitalWrite(latchPin, 0);
    shiftOut(dataPin, clockPin, dataGREEN3);
    shiftOut(dataPin, clockPin, dataRED3);
    digitalWrite(latchPin, 1);
    delay(300);
  }
  }
  return 0;
}
