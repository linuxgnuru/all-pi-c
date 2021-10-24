#include <stdio.h>
#include <iostream>
#include <cstring>
#include <wiringPi.h>
#include <wiringPiSPI.h>

#define CE 0

using namespace std;

class RasPiSPI {
    private:
        unsigned char *TxBuffer;
        int TxBufferIndex;

    public:
        RasPiSPI();
        ~RasPiSPI();

        void begin() { begin(CE, 1000000); }
        void begin(int, int);

        void transfer(char);
        void endTransfer();
};

RasPiSPI::RasPiSPI() {
    TxBuffer = new unsigned char[1024];
    TxBufferIndex = 0;
}

RasPiSPI::~RasPiSPI() { delete[] TxBuffer; }

void RasPiSPI::begin(int channel, int speed)
{
    if (wiringPiSPISetup(channel, speed) < 0) cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
}

void RasPiSPI::transfer(char c)
{
    TxBuffer[TxBufferIndex] = c;
    TxBufferIndex++;
}

void RasPiSPI::endTransfer()
{
    wiringPiSPIDataRW(CE, TxBuffer, TxBufferIndex);
    TxBufferIndex = 0;
}

const char Digits[8] = {
 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
};

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp        = 0x00;
const char DecodeMode  = 0x09;
const char Intensity   = 0x0A;
const char ScanLimit   = 0x0B;
const char ShutDown    = 0x0C;
const char DisplayTest = 0x0F;

const char numOfDevices = 4;

void setup();

RasPiSPI SPI;

int main(int argc, char **argv)
{
	setup();
	return 0;
}

void SetData(char adr, char data, char device)
{
	for (int i = numOfDevices; i > 0; i--)
	{
		if ((i == device) || (device == 255))
		{
			SPI.transfer(adr);
			SPI.transfer(data);
		}
		else
		{
			SPI.transfer(NoOp);
			SPI.transfer(0);
		}
	}
	SPI.endTransfer();
    delay(1);
}

void SetData(char adr, char data) { SetData(adr, data, 255); }
void SetShutDown(char Mode) { SetData(ShutDown, !Mode); }
void SetScanLimit(char Digits) { SetData(ScanLimit, Digits); }
void SetIntensity(char intense) { SetData(Intensity, intense); }
void SetDecodeMode(char Mode) { SetData(DecodeMode, Mode); }

void setup()
{
    SPI.begin();
    SetDecodeMode(false);
    SetScanLimit(7);
    SetIntensity(5);
    for (int j = 1; j < 5; j++)
        SetData(j, 0x0, 255);
    //SetShutDown(true);
}

