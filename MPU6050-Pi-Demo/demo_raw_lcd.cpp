#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <string.h> 
#include <errno.h>
#include <unistd.h>

#include <wiringPi.h>
#include <lcd.h>

#include "I2Cdev.h"
#include "MPU6050.h"

int lcdFD;

const int RS = 3;
const int EN = 14;
const int D0 = 4;
const int D1 = 12;
const int D2 = 13;
const int D3 = 6;

static void die(int sig)
{
    if (sig != 0 && sig != 2)
        (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2)
        (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup()
{
    // initialize device
    printf("Initializing I2C devices...\n");
    accelgyro.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(accelgyro.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");
}

void loop()
{
    char buf[32];
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);

    // display accel/gyro x/y/z values
    //printf("a/g: %6hd %6hd %6hd   %6hd %6hd %6hd\n",ax,ay,az,gx,gy,gz);
    sprintf(buf, "%6hd %6hd %6hd", ax, ay, az);
    lcdPosition(lcdFD, 0, 0);
    lcdPrintf(lcdFD, buf);
    sprintf(buf, "%6hd %6hd %6hd", gx, gy, gz);
    lcdPosition(lcdFD, 0, 1);
    lcdPrintf(lcdFD, buf);
}

int main()
{
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    if (wiringPiSetup() < 0)
    {
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1 ;
    }
    lcdFD = lcdInit(2, 16, 4, RS, EN, D0, D1, D2, D3, D0, D1, D2, D3);
    setup();
    for (;;)
    {
        loop();
    }
}

