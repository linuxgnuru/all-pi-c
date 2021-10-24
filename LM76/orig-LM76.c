#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <wiringPi.h>

#include "LM76.h"

#define ADDRESS 0x48

static const char *devName = "/dev/i2c-1";
int i2c_file;

void getData(int my_cmd);

void initLM76()
{
    unsigned char cmd[16];

    cmd[0] = 0x01;
    cmd[1] = 0x01;
    write(i2c_file, cmd, 1);
    cmd[1] = 0x00;
    write(i2c_file, cmd, 1);
}

static void die(int sig)
{
    close(i2c_file);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
        exit(1);
    }
    if ((i2c_file = open(devName, O_RDWR)) < 0)
    {
        fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to access %s\n", __LINE__, __FILE__, __func__, devName);
        exit(1);
    }
    if (ioctl(i2c_file, I2C_SLAVE, ADDRESS) < 0)
    {
        fprintf(stderr, "[%d] [%s] [%s] I2C: Failed to acquire bus access/talk to slave 0x%x\n", __LINE__, __FILE__, __func__, NSE_ADDRESS);
        exit(1);
    }
    initLM76();
    sendI2C(G_TEMP);
    close(i2c_file);
    return EXIT_SUCCESS;
}

int sendI2C(int u_cmd)
{
    unsigned char cmd[16];
    int Thys, Tcrit, Tlow, Thigh;
    float Temp = 0.0;
    int t;

    switch (u_cmd)
    {
        case G_TEMP:
            cmd[0] = 0x00;
            if (write(i2c_file, cmd, 1) == 1)
            {
                usleep(10000); // wait for messages to be sent
                char buf[1];
                if (read(i2c_file, buf, 1) == 1)
                {
                    Temp = (float)buf[0];
                    t = (buf[0] & 0x0080) >> 7;
                    Temp = Temp &0xFF7F;
                    Temp = ((-1)**t)*(((((Temp &0x007F)<<8)|((Temp&0xFF00)>>8))>>3)*0.0625);
                    printf("Temperature: %0.2f C\n", Temp);
                }
            }
            break;
        case G_THYS:
            break;
        case G_T_CRIT:
            break;
        case G_T_LOW:
            break;
        case G_T_HIGH:
            break;
        case G_CONF:
            break;
        case S_CONF:
            break;
        case S_THYS:
            break;
        case S_T_CRIT:
            break;
        case S_T_LOW:
            break;
        case S_T_HIGH:
            break;
        default:
            break;
    }
}
