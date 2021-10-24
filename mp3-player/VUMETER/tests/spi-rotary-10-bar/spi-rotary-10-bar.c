#include <stdio.h>
#include <stdlib.h>

// for signal catching
#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <wiringPi.h>
#include <wiringPiSPI.h>

#ifndef	TRUE
#  define	TRUE  (1==1)
#  define	FALSE (1==2)
#endif

// Rotary Encoder
#define encoderPinB 29
#define encoderPinA 25
#define encoderButton 28

#define CE 0
#define NUM_CHIPS 10

const int extraLedPins[] = { 1, 0 }; // for bar graph leds 9 and 10

const long debounceDelay = 50;

unsigned char data[NUM_CHIPS] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
unsigned char backup_data[NUM_CHIPS] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

void bitWrite(int ab, int n, int b)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (-b ^ data[ab]) & (1 << n);
}

void bitClear(int ab, int n)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (0 ^ data[ab]) & (1 << n);
}

void bitSet(int ab, int n)
{
    if (ab < 0 || ab > 1) return;
    if (n <= 7 && n >= 0) data[ab] ^= (-1 ^ data[ab]) & (1 << n);
}

void Off()
{
    data[0] = data[1] = 0x0;
    backup_data[0] = backup_data[1] = data[0];
    wiringPiSPIDataRW(CE, data, 2);
    data[0] = data[1] = backup_data[0];
    digitalWrite(extraLedPins[0], LOW);
    digitalWrite(extraLedPins[1], LOW);
}

void On()
{
    data[0] = 0b11111111;
    backup_data[0] = data[0];
    wiringPiSPIDataRW(CE, data, 1);
    data[0] = backup_data[0];
    digitalWrite(extraLedPins[0], HIGH);
    digitalWrite(extraLedPins[1], HIGH);
}

void doGraph(int num)
{
    int thisLed;
    int lednum;
    _Bool toggle;

    if (num < 0 || num > 10) return;
    else if (num == 0) Off();
    else if (num == 10) On();
    else
    {
        for (thisLed = 0; thisLed < 10; thisLed++)
        {
            lednum = thisLed;
            toggle = (thisLed < num);
            if (thisLed < 8)
            {
                lednum = abs(lednum - 7);
                bitWrite(lednum, toggle);
                backup_data[0] = data[0];
                wiringPiSPIDataRW(CE, data, 1);
                data[0] = backup_data[0];
            }
            else
            {
                lednum -= 8;
                digitalWrite(extraLedPins[lednum], toggle);
            }
        }
    }
}

void die(int sig)
{
  Off();
  if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
  if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
  exit(0);
}

int main(int argc, char **argv)
{
  _Bool reading;
  _Bool quitFlag = FALSE;
  _Bool flipflopflag;
  _Bool lastStatus;
  _Bool currentStatus;
  long lastDebounceTime = 0;
  int buttonState = HIGH;
  int lastButtonState = HIGH;
  int number = 0;

  (void)signal(SIGINT, die);
  (void)signal(SIGHUP, die);
  (void)signal(SIGTERM, die);
  (void)signal(SIGABRT, die);
  if (wiringPiSetup() == -1)
  {
    fprintf(stdout, "[%s:%d] [%s] Error trying to setup wiringPi - oops: %s\n", __FILE__, __LINE__, __func__, strerror(errno));
    exit(1);
  }
  wiringPiSPISetup(CE, 500000);
  pinMode(encoderButton, INPUT);
  pullUpDnControl(encoderButton, PUD_UP);
  pinMode(encoderPinB, INPUT);
  pinMode(encoderPinA, INPUT);
  Off();
  while (quitFlag == FALSE)
  {
    reading = digitalRead(encoderButton);
    if (reading != lastButtonState)
      lastDebounceTime = millis();
    if ((millis() - lastDebounceTime) > debounceDelay)
    {
      if (reading != buttonState)
      {
        buttonState = reading;
        if (buttonState == LOW)
          quitFlag = TRUE;
      }
    }
    lastButtonState = reading;

    lastStatus = digitalRead(encoderPinB);
    while (!digitalRead(encoderPinA))
    {
      currentStatus = digitalRead(encoderPinB);
      flipflopflag = TRUE;
    }
    if (flipflopflag == TRUE)
    {
      flipflopflag = FALSE;
      if (!lastStatus && currentStatus) number++;
      if (lastStatus && !currentStatus) number--;
      if (number < 0) number = 0;
      if (number > 10) number = 10;
      doGraph(number);
    }
  }
  Off();
  return EXIT_SUCCESS;
}

