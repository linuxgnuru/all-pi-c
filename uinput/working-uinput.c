#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/input.h>
#include <linux/uinput.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define die(str, args...) do { \
    perror(str); \
    exit(EXIT_FAILURE); \
} while (0)

#define CE 1

const _Bool leds[18][8] = {
   // F  G  E  D  B  A  C  dp
    { 1, 0, 1, 1, 1, 1, 1, 0 }, // 0
    { 0, 0, 0, 0, 1, 0, 1, 0 }, // 1
    { 0, 1, 1, 1, 1, 1, 0, 0 }, // 2
    { 0, 1, 0, 1, 1, 1, 1, 0 }, // 3
    { 1, 1, 0, 0, 1, 0, 1, 0 }, // 4
    { 1, 1, 0, 1, 0, 1, 1, 0 }, // 5
    { 1, 1, 1, 1, 0, 1, 1, 0 }, // 6
    { 0, 0, 0, 0, 1, 1, 1, 0 }, // 7
    { 1, 1, 1, 1, 1, 1, 1, 0 }, // 8
    { 1, 1, 0, 1, 1, 1, 1, 0 }, // 9
    { 1, 1, 1, 0, 1, 1, 1, 1 }, // A. 10 (A)
    { 1, 1, 1, 1, 1, 1, 1, 1 }, // B. 11 (8)
    { 1, 0, 1, 1, 0, 1, 0, 1 }, // C. 12 (C)
    { 1, 0, 1, 1, 1, 1, 1, 1 }, // D. 13 (0) 
    { 1, 1, 1, 0, 1, 0, 1, 1 }, // #. 14 (#)
    { 0, 1, 0, 1, 0, 1, 0, 1 }, // *. 15 (*)
    { 1, 1, 1, 1, 0, 1, 0, 1 }, // E. 16 (error)
    { 0, 0, 0, 0, 0, 0, 0, 0 }  //    17 (clear)
};

const char MATRIX[4][4] = {
    { '1','2','3','A' },
    { '4','5','6','B' },
    { '7','8','9','C' },
    { '*','0','#','D' }
};

// Pins for keypad
const int ROW[] = { 0, 1, 2, 3 };
const int COL[] = { 4, 5, 6, 7 };

// Pins for LEDs
const int okPin = 21;
const int noPin = 22;

// Password
const char pass[5] = { '1', '7', '0', '1', 'D' };

// device
int fd;

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

void bitWrite(int n, int b) { data[0] ^= (-b ^ data[0]) & (1 << n); }

void bitClear(int n) { data[0] ^= (0 ^ data[0]) & (1 << n); }

void bitSet(int n) { data[0] ^= (-1 ^ data[0]) & (1 << n); }

// catch signals and clean up GPIO
static void sDie(int sig)
{
    int i;

    wiringPiSPIDataRW(CE, 0x0, 1);
    digitalWrite(okPin, LOW);
    digitalWrite(noPin, LOW);
    for (i = 0; i < 4; i++)
        digitalWrite(COL[i], LOW);
    if (sig != 0 && sig != 2)
        (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2)
        (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    exit(0);
}

void printLed(int num)
{
    for (int i = 0; i < 8; i++)
    {
        //bitWrite(num, leds[num][i]);
        if (leds[num][i] == 1)
            bitSet(i);
        else
            bitClear(i);
        backup_data[0] = data[0];
        wiringPiSPIDataRW(CE, data, 1);
        data[0] = backup_data[0];
    }
}

void pDigit(char *comp)
{
    int digit = -1;

    //printf("%c\n", comp[0]);
    //if (strcmp(comp, "#") == 0 || strcmp(comp, "*") == 0 || strcasecmp(comp, "A") == 0 || strcasecmp(comp, "B") == 0 || strcasecmp(comp, "C") == 0 || strcasecmp(comp, "D") == 0 || strcasecmp(comp, "E") == 0)
    //{
         if (strcasecmp(comp, "A") == 0) digit = 10;
    else if (strcasecmp(comp, "B") == 0) digit = 11;
    else if (strcasecmp(comp, "C") == 0) digit = 12;
    else if (strcasecmp(comp, "D") == 0) digit = 13;
    else if (    strcmp(comp, "#") == 0) digit = 14;
    else if (    strcmp(comp, "*") == 0) digit = 15;
    else if (strcasecmp(comp, "E") == 0) digit = 16;
    //}
    else digit = atoi(comp);
    printLed(digit);
    /*
    for (i = 0; i < 8; i++)
    {
            if (leds[digit][i] != 30)
                digitalWrite(addr + leds[digit][i], HIGH);
            if (leds[digit][7] == 40)
                digitalWrite(addr + 7, HIGH);
            else
                digitalWrite(addr + 7, LOW);
    }
    */
}

int main(int argc, char **argv)
{
    int i;
    struct uinput_user_dev uidev;
    struct input_event ev;
    int pNum = 0;
    int loopFlag = 1;
    int numTries = 0;
    char new_m[2];
    char buf[80];

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, sDie);
    (void)signal(SIGHUP, sDie);
    if (wiringPiSetup() == -1)
    {
        fprintf(stderr, "oops: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    wiringPiSPISetup(CE, 500000);
    pinMode(okPin, OUTPUT);
    pinMode(noPin, OUTPUT);
    digitalWrite(okPin, LOW);
    digitalWrite(noPin, LOW);
    for (i = 0; i < 4; i++)
        pinMode(COL[i], OUTPUT);
    for (i = 0; i < 4; i++)
    {
        pinMode(ROW[i], INPUT);
        pullUpDnControl(ROW[i], PUD_UP);
    }
    // clear led
    printLed(17);
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        sprintf(buf, "[%d, %s] error: open", __LINE__, __FILE__);
        die(buf);
    }
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_L);
    ioctl(fd, UI_SET_KEYBIT, KEY_1);
    ioctl(fd, UI_SET_KEYBIT, KEY_N);
    //ioctl(fd, UI_SET_KEYBIT, KEY_LEFTSHIFT);
    //ioctl(fd, UI_SET_KEYBIT, KEY_1);
    ioctl(fd, UI_SET_KEYBIT, KEY_U);
    ioctl(fd, UI_SET_KEYBIT, KEY_X);
    //ioctl(fd, UI_SET_KEYBIT, KEY_Y);
    ioctl(fd, UI_SET_KEYBIT, KEY_ENTER);
    memset(&uidev, 0, sizeof(uidev));
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "RaspberryPi-KeyPass");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor = 0x1234;
    uidev.id.product = 0xfedc;
    uidev.id.version = 1;
    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
    sleep(2);
    printLed(17);
    while (loopFlag)
    {
        for (i = 0; i < 4; i++)
        {
            digitalWrite(COL[i], LOW);
            for (int j = 0; j < 4; j++)
            {
                if (digitalRead(ROW[j]) == LOW)
                {
                    new_m[0] = MATRIX[j][i];
                    new_m[1] = '\0';
                    pDigit(new_m);
                    if (MATRIX[j][i] == '#' || MATRIX[j][i] == '*')
                    {
                        digitalWrite(okPin, LOW);
                        digitalWrite(noPin, LOW);
                        printLed(17);
                        numTries = 0;
                        pNum = 0;
                    }
                    else
                    {
                        if (MATRIX[j][i] == pass[pNum])
                            pNum++;
                        else
                        {
                            pNum = 0;
                            digitalWrite(noPin, HIGH);
                            new_m[0] = 'E';
                            new_m[1] = '\0';
                            pDigit(new_m);
                            numTries++;
                        }
                        if (pNum >= 5 && digitalRead(noPin) != HIGH)
                            loopFlag = 0;
                    }
                    while (digitalRead(ROW[j]) == LOW)
                        delay(20);
                }
            }
            digitalWrite(COL[i], HIGH);
        }
    }
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_O;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_H;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_N;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_LEFTSHIFT;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_1;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_LEFTSHIFT;
    ev.value = 0;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_J;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_A;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_Y;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = KEY_ENTER;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));

    usleep(15000);

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    write(fd, &ev, sizeof(struct input_event));

    //sleep(2);
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    printLed(17);
    digitalWrite(okPin, HIGH);
    return EXIT_SUCCESS;
}
