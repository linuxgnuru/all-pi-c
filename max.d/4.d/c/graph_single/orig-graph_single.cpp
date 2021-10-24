/*        (y) (g) (b) (k) (r)
           C   L   D   G   V
           |   |   |   |   |
           |   |   |   |   |
     7   6   5   4   3   2   1   0
   +---+---+---+---+---+---+---+---+
 7 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 6 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 5 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 4 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 3 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
 0 |   |   |   |   |   |   |   |   |
   +---+---+---+---+---+---+---+---+
           |   |   |   |   |
           |   |   |   |   |

*/
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <signal.h>

#include <string.h> // for fprintf

#include <time.h> // for srand

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define uchar unsigned char
#define uint unsigned int

/*
useage:
printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
 */

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    (byte & 0x80 ? '1' : '0'), \
    (byte & 0x40 ? '1' : '0'), \
    (byte & 0x20 ? '1' : '0'), \
    (byte & 0x10 ? '1' : '0'), \
    (byte & 0x08 ? '1' : '0'), \
    (byte & 0x04 ? '1' : '0'), \
    (byte & 0x02 ? '1' : '0'), \
    (byte & 0x01 ? '1' : '0') 

unsigned char spidata[16];
unsigned char status[64];

/*
uchar data[8] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };

void bitWrite(int col, int row, int b) { data[col] ^= (-b ^ data[col]) & (1 << row); }
void bitClear(int col, int row) { data[col] ^= (0 ^ data[col]) & (1 << row); }
void bitSet(int col, int row) { data[col] ^= (-1 ^ data[col]) & (1 << row); }
*/

void writeMax(unsigned char addr, unsigned char opcode, unsigned char dat)
{
    int offset = addr * 2;
    int maxbytes = 8; // maxDevices (4) * 2

    for (int i = 0; i < maxbytes; i++)
        spidata[i] = 0x0;
    spidata[offset + 1] = opcode;
    spidata[offset] = dat;
    wiringPiSPIDataRW(CE, spidata, 16);
}

void clearMax(int addr);

void Init_MAX7219()
{
    for (int i = 0; i < 4; i++)
    {
        writeMax(i, 0x09, 0x00); //  9 decode mode
        writeMax(i, 0x0a, 0x03); // 10 intensity
        writeMax(i, 0x0b, 0x07); // 11 scanlimit
        writeMax(i, 0x0c, 0x01); // 12 shutdown
        writeMax(i, 0x0f, 0x00); // 15 displaytest
        clearMax(i);
    }
}

void clearMax(int addr)
{
    int offset;

    if (addr == -1) // clear all
    {
        for (int i = 0; i < 4; i++)
        {
            offset = i * 8;
            for (int j = 0; j < 8; j++)
            {
                status[offset + j] = 0;
                writeMax(addr, j + 1, status[offset + j]);
            }
        }
    }
    else
    {
        offset = addr * 8;
        for (int i = 0; i < 8; i++)
        {
            status[offset + i] = 0;
            writeMax(addr, i + 1, status[offset + i]);
        }
    }
}

void setLed(int addr, int row, int column, _Bool state)
{
    int offset;
    unsigned char val = 0x00;

    if (addr < 0 || addr >= 4) return;
    if (row < 0 || row > 7 || column < 0 || column > 7) return;
    offset = addr * 8;
    val = 0b10000000 >> column;
    if (state)
        status[offset + row] = status[offset + row] | val;
    else
    {
        val =~ val;
        status[offset + row] = status[offset + row] & val;
    }
    writeMax(addr, row + 1, status[offset + row]);
}

void doGraph(int addr, int c, int r)
{
    //c = 7 - c;
    for (int i = 0; i < 8; i++)
    {
        //if (r == 0) data[c] = 0x0;
        //else bitWrite(c, i, (i < r));
        setLed(addr, c, i, (i < r));
    }
}

static void die(int sig)
{
    if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig));
    if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig));
    exit(0);
}

int main(int argc, char **argv)
{
    //int col[32];
    int addr;
    int col;
    int row;

    // note: we're assuming BSD-style reliable signals here
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    srand(time(NULL));
    if (argc != 3 && argc != 4)
    {
        fprintf(stdout, "usage: %s addr col (row)\n", argv[0]);
        fprintf(stdout, "addr = 0 - 3, col = 0 - 7, row optional 0 - 7\n");
        return EXIT_FAILURE;
    }
    if (wiringPiSetup() == -1)
    {
        fprintf(stdout, "[%d] [%s] [%s] Error trying to setup wiringPi - oops: %s\n", __LINE__, __FILE__, __func__, strerror(errno));
        exit(1);
    }
    wiringPiSPISetup(CE, 500000);
    Init_MAX7219();

    addr = atoi(argv[1]);
    col = atoi(argv[2]);

    row = (argc == 4 ? atoi(argv[3]) : rand() % 8);
    printf("addr %d col %d row %d\n", addr, col, row);
    doGraph(addr, col, row);
    setLed(addr, col, row, 1);
    /*
       for (i = 0; i < 8; i++)
       {
       for (j = 0; j < 8; j++)
       {
       bitSet(j, i);
    //backup_data[j] = data[j];
    writeMax(j + 1, data[j]);
    //data[j] = backup_data[j];
    delay(100);
    }
    }
     */
    return EXIT_SUCCESS;
}

