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

#define CE 0

const _Bool leds[18][8] = {
   // B  A  F  G  D  C  .  E (new)
    { 1, 1, 1, 0, 1, 1, 0, 1 }, // 0
    { 1, 0, 0, 0, 0, 1, 0, 0 }, // 1
    { 1, 1, 0, 1, 1, 0, 0, 1 }, // 2
    { 1, 1, 0, 1, 1, 1, 0, 0 }, // 3
    { 1, 0, 1, 1, 0, 1, 0, 0 }, // 4
    { 0, 1, 1, 1, 1, 1, 0, 0 }, // 5
    { 0, 1, 1, 1, 1, 1, 0, 1 }, // 6
    { 1, 1, 0, 0, 0, 1, 0, 0 }, // 7
    { 1, 1, 1, 1, 1, 1, 0, 1 }, // 8
    { 1, 1, 1, 1, 1, 1, 0, 0 }, // 9
    { 1, 1, 1, 1, 0, 1, 1, 1 }, // A. 10 (A)
    { 0, 0, 1, 1, 1, 1, 1, 1 }, // B. 11 (b)
    { 0, 1, 1, 0, 1, 0, 1, 1 }, // C. 12 (c)
    { 1, 0, 0, 1, 1, 1, 1, 1 }, // D. 13 (d)
    { 0, 1, 1, 1, 1, 0, 0, 1 }, // E  14 (error)
    { 1, 0, 1, 1, 0, 1, 1, 1 }, // #. 15 (#) not used
    { 0, 1, 0, 1, 1, 0, 1, 0 }  // *. 16 (*) not used
};

const char MATRIX[4][4] = {
    { '1','2','3','A' },
    { '4','5','6','B' },
    { '7','8','9','C' },
    { '*','0','#','D' }
};

struct uinput_user_dev uidev;
struct input_event ev;

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

int pNum = 0;
//int numTries = 0;

unsigned char data[1] = { 0x0 };
unsigned char backup_data[1] = { 0x0 };

// catch signals and clean up GPIO
static void sDie(int sig)
{
    digitalWrite(okPin, LOW);
    digitalWrite(noPin, LOW);
    ioctl(fd, UI_DEV_DESTROY);
    close(fd);
    wiringPiSPIDataRW(CE, 0x0, 1);
    for (int i = 0; i < 4; i++) digitalWrite(COL[i], LOW);
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %d\n", sig);
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C\n");
    exit(0);
}

void initUinput()
{
    fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0)
    {
        fprintf(stderr, "[%d, %s] error: open", __LINE__, __FILE__);
        exit(EXIT_FAILURE);
    }
    ioctl(fd, UI_SET_EVBIT, EV_KEY);

    ioctl(fd, UI_SET_KEYBIT, KEY_P);
    ioctl(fd, UI_SET_KEYBIT, KEY_I);

    ioctl(fd, UI_SET_KEYBIT, KEY_L);
    ioctl(fd, UI_SET_KEYBIT, KEY_1);
    ioctl(fd, UI_SET_KEYBIT, KEY_N);
    ioctl(fd, UI_SET_KEYBIT, KEY_U);
    ioctl(fd, UI_SET_KEYBIT, KEY_X);
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
    digitalWrite(noPin, HIGH);
    digitalWrite(okPin, HIGH);
    delay(50);
    digitalWrite(okPin, LOW);
    digitalWrite(noPin, LOW);
}

void writeUinput(int k)
{
    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_KEY;
    ev.code = k;
    ev.value = 1;
    write(fd, &ev, sizeof(struct input_event));
}

void enterUser()
{
    writeUinput(KEY_P);
    writeUinput(KEY_I);
    writeUinput(KEY_ENTER);

    usleep(15000);

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    write(fd, &ev, sizeof(struct input_event));
    ioctl(fd, UI_DEV_DESTROY);
}

void enterPassword()
{
    writeUinput(KEY_L);
    writeUinput(KEY_1);
    writeUinput(KEY_N);
    writeUinput(KEY_U);
    writeUinput(KEY_X);
    writeUinput(KEY_ENTER);

    usleep(15000);

    memset(&ev, 0, sizeof(struct input_event));
    ev.type = EV_SYN;
    ev.code = 0;
    ev.value = 0;
    write(fd, &ev, sizeof(struct input_event));
    ioctl(fd, UI_DEV_DESTROY);
    digitalWrite(okPin, HIGH);
    delay(250);
}

void bitWrite(int n, int b) { data[0] ^= (-b ^ data[0]) & (1 << n); }

void bitClear(int n) { data[0] ^= (0 ^ data[0]) & (1 << n); }

void bitSet(int n) { data[0] ^= (-1 ^ data[0]) & (1 << n); }

void printLed(int num)
{
    for (int i = 0; i < 8; i++)
    {
        if (leds[num][i] == 1) bitSet(i);
        else bitClear(i);
        backup_data[0] = data[0];
        wiringPiSPIDataRW(CE, data, 1);
        data[0] = backup_data[0];
    }
}

void pDigit(char *comp)
{
    int digit = -1;

         if (strcasecmp(comp, "A") == 0) digit = 10;
    else if (strcasecmp(comp, "B") == 0) digit = 11;
    else if (strcasecmp(comp, "C") == 0) digit = 12;
    else if (strcasecmp(comp, "D") == 0) digit = 13;
    else if (strcasecmp(comp, "E") == 0) digit = 14;
    //else if (    strcmp(comp, "#") == 0) digit = 14;
    //else if (    strcmp(comp, "*") == 0) digit = 15;
    //else if (strcasecmp(comp, "E") == 0) digit = 16;
    else digit = atoi(comp);
    printLed(digit);
}

void resetAll()
{
    digitalWrite(okPin, LOW);
    digitalWrite(noPin, LOW);
    wiringPiSPIDataRW(CE, 0x0, 1);
    pNum = 0;
}

int main(int argc, char **argv)
{
    int i;
    char new_m[2];

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, sDie);
    (void)signal(SIGHUP, sDie);
    if (wiringPiSetup() == -1)
    {
        fprintf(stderr, "oops: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }
    wiringPiSPISetup(CE, 500000);
    // Setup our priority
    piHiPri(20);
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
#ifdef DEMO
    for (i = 0; i < 15; i++)
    {
        printLed(i);
        delay(100);
    }
#endif
    //sleep(2);
    initUinput();
    // clear led
    wiringPiSPIDataRW(CE, 0x0, 1);
    while (1)
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
                    printf("%s\n", new_m);
                    if (MATRIX[j][i] == '#')
                    {
                        enterUser();
                        initUinput();
                    }
                    else if (MATRIX[j][i] == '*')
                    {
                        resetAll();
                    }
                    else
                    {
                        pDigit(new_m);
                        if (MATRIX[j][i] == pass[pNum])
                        {
                            pNum++;
                        }
                        else
                        {
                            pNum = 0;
                            digitalWrite(noPin, HIGH);
                            //new_m[0] = 'E';
                            //new_m[1] = '\0';
                            //pDigit(new_m);
                        }
                        if (pNum >= 5 && digitalRead(noPin) != HIGH)
                        {
                            enterPassword();
                            resetAll();
                            initUinput();
                        }
                    }
                    while (digitalRead(ROW[j]) == LOW)
                    {
                        delay(20);
                    }
                }
            }
            digitalWrite(COL[i], HIGH);
        }
    }
    return EXIT_SUCCESS;
}

