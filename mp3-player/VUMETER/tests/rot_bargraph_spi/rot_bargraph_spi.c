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

const long debounceDelay = 50;

unsigned char data[2] = { 0x0, 0x0 };
unsigned char backup_data[2] = { 0x0, 0x0 };

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
    backup_data[0] = data[0];
    backup_data[1] = data[1];
    wiringPiSPIDataRW(0, data, 2);
    data[0] = backup_data[0];
    data[1] = backup_data[1];
}

void Off_r()
{
    data[1] = 0b00000000;
    backup_data[0] = data[0];
    backup_data[1] = data[1];
    wiringPiSPIDataRW(0, data, 2);
    data[0] = backup_data[0];
    data[1] = backup_data[1];
}

void Off_l()
{
    data[0] = 0b00000000;
    backup_data[0] = data[0];
    backup_data[1] = data[1];
    wiringPiSPIDataRW(0, data, 2);
    data[0] = backup_data[0];
    data[1] = backup_data[1];
}

void doGraph(int num_r, int num_l)
{
    int i, thisLed_l, thisLed_r;
    _Bool toggle_l, toggle_r;
    _Bool con_l, con_r;

    con_l = con_r = 1;
    if (num_r < 0 || num_r > 8) return;
    else if (num_l < 0 || num_l > 8) return;
    else
    {
        // left side
        for (i = 0; i < 8; i++)
        {
            thisLed_r = i;
            thisLed_l = abs(7 - i);
            if (num_r == 0)
            {
                Off_r(); 
                con_r = 0;
            }
            if (num_l == 0)
            {
                Off_l();
                con_l = 0;
            }
            if (con_l)
            {
                toggle_l = (thisLed_l < num_l);
                bitWrite(0, thisLed_l, toggle_l);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
            }
            if (con_r)
            {
                toggle_r = (thisLed_l < num_r);
                bitWrite(1, thisLed_r, toggle_r);
                backup_data[0] = data[0];
                backup_data[1] = data[1];
                wiringPiSPIDataRW(0, data, 2);
                data[0] = backup_data[0];
                data[1] = backup_data[1];
            }
            con_l = con_r = 1;
        }
    }
}

static void die(int sig)
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
  wiringPiSPISetup(0, 500000);
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
      if (number > 8) number = 8;
      doGraph(number, number);
    }
  }
  Off();
  return EXIT_SUCCESS;
}

