#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#define CE 0
const char NoOp        = 0x00;
const char Digits[8] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08 };
const char DecodeMode  = 0x09;
const char Intensity   = 0x0A;
const char ScanLimit   = 0x0B;
const char ShutDown    = 0x0C;
const char DisplayTest = 0x0F;
const char numOfDevices = 4;
unsigned char ledData[4][8] = { { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 }, { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 } };
unsigned char TxBuffer[1024];
int TxBufferIndex = 0;
void transfer(char);
void endTransfer();
void begin(int channel, int speed) { if (wiringPiSPISetup(channel, speed) < 0) printf("Failed to open SPI port %d! Please try with sudo\n", channel); }
void transfer(char c) { TxBuffer[TxBufferIndex] = c; TxBufferIndex++; }
void endTransfer() { wiringPiSPIDataRW(CE, TxBuffer, TxBufferIndex); TxBufferIndex = 0; }
void setup();
void bitWrite(int addr, int col, int row, int b) { if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 7) return; ledData[addr][col] ^= (-b ^ ledData[addr][col]) & (1 << row); }
void bitClear(int addr, int col, int row) { if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 7) return; ledData[addr][col] ^= (0 ^ ledData[addr][col]) & (1 << row); }
void bitSet(int addr, int col, int row) { if (addr < 0 || addr > 3 || col < 0 || col > 7 || row < 0 || row > 7) return; ledData[addr][col] ^= (-1 ^ ledData[addr][col]) & (1 << row); }
_Bool bitRead(int addr, int col, int row) { return (ledData[addr][col] >> row) & 0x01; }
void SetData(char adr, char data, char device) { for (int i = numOfDevices; i > 0; i--) { if ((i == device) || (device == 255)) { transfer(adr); transfer(data); } else { transfer(NoOp); transfer(0); } } endTransfer(); }
void SetData_all(char adr, char data) { SetData(adr, data, 255); }
void SetShutDown(char Mode) { SetData_all(ShutDown, !Mode); }
void SetScanLimit(char Digits) { SetData_all(ScanLimit, Digits); }
void SetIntensity(char intense) { SetData_all(Intensity, intense); }
void SetDecodeMode(char Mode) { SetData_all(DecodeMode, Mode); }
void clearAll() { for (int i = 0; i < 8; i++) SetData_all(Digits[i], 0b00000000); }
void Draw(int addr, int col, int row, _Bool b) { char mydata = 0x0; if (addr == 2) { col = abs(col - 7); row = abs(row - 7); } bitWrite(addr, col, row, b); mydata = ledData[addr][col]; SetData(Digits[col], mydata, addr + 1); }
void doGraph(int addr, int height, int col) { if (height == 0) for (int i = 0; i < 8; i++) Draw(addr, i, col, 0); else if (height == 8) for (int i = 0; i < 8; i++) Draw(addr, i, col, 1); else for (int i = 7; i > -1; i--) Draw(addr, i, col, (i < height)); }
static void die(int sig);
int main(int argc, char **argv) {
    (void)signal(SIGINT, die);
    (void)signal(SIGHUP, die);
    (void)signal(SIGTERM, die);
    (void)signal(SIGABRT, die);
    srand((unsigned)time(NULL));
    setup();
    for (int addr = 3; addr > -1; addr--) { for (int col = 0; col < 7; col++) { int ran = rand() % 9; doGraph(addr, ran, col); } }
    return EXIT_SUCCESS;
}
void setup() { begin(CE, 1000000); SetDecodeMode(FALSE); SetScanLimit(7); SetIntensity(3); SetShutDown(FALSE); clearAll(); }
static void die(int sig) { clearAll(); SetShutDown(1); if (sig != 0 && sig != 2) (void)fprintf(stderr, "caught signal %s\n", strsignal(sig)); if (sig == 2) (void)fprintf(stderr, "Exiting due to Ctrl + C (%s)\n", strsignal(sig)); exit(0); }

