#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>

#include <wiringPi.h>
#include <softPwm.h>

#define TRUE_ 1
#define FALSE_ 0

const int sensorLeftPin = 4;
const int sensorRightPin = 5;

// Motor stuff
const int enable1Pin = 23;
const int in1Pin = 24;
const int in2Pin = 25;

const int enable2Pin = 27;
const int in3Pin = 28;
const int in4Pin = 29;

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

void usage(char *name)
{
    printf("usage: %s [motor number - 0/1 (right or left)] [speed ]", name);
}

void runMotor(int dir, int speed)
{
    if (dir == 0)
    {
        digitalWrite(in1Pin, HIGH);
        digitalWrite(in2Pin, LOW);
        softPwmWrite(enable1Pin, speed);
        digitalWrite(in3Pin, HIGH);
        digitalWrite(in4Pin, LOW);
        softPwmWrite(enable2Pin, speed);
    }
    if (dir == 1)
    {
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, HIGH);
        softPwmWrite(enable1Pin, speed);
        digitalWrite(in3Pin, LOW);
        digitalWrite(in4Pin, HIGH);
        softPwmWrite(enable2Pin, speed);
    }
}

int main(int argc, char **argv)
{
    int badFlag = FALSE_;
    int direction;
    int speed;

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);

    if (argc > 2)
    {
        //if (strcmp(argv[1], "#") == 0) digit = 14;
        //digit = atoi(argv[1]);
        direction = atoi(argv[1]);
        speed = atoi(argv[2]);
        if (speed < 0 || speed > 100)
            badFlag = TRUE_;
        if (direction != 0 && direction != 1)
            badFlag = TRUE_;
    }
    else
        badFlag = TRUE_;
    if (badFlag == TRUE_)
    {
        usage(argv[0]);
        return EXIT_FAILURE;
    }
    if (wiringPiSetup () == -1)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }
    pinMode(sensorLeftPin, INPUT);
    pinMode(sensorRightPin, INPUT);
    pinMode(in1Pin, OUTPUT);
    pinMode(in2Pin, OUTPUT);
    pinMode(in3Pin, OUTPUT);
    pinMode(in4Pin, OUTPUT);
    if (softPwmCreate(enable1Pin, 0, 100) != 0)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }
    if (softPwmCreate(enable2Pin, 0, 100) != 0)
    {
        fprintf(stdout, "oops: %s\n", strerror(errno));
        return 1;
    }
    runMotor(direction, speed);
    delay(1000);
    runMotor(0, 0);
    return EXIT_SUCCESS;
}
