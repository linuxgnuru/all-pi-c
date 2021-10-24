#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#include "RasPiSPI.h"

using namespace std;

RasPiSPI::RasPiSPI()
{
    TxBuffer = new unsigned char[1024]; // Buffer for TxData
    TxBufferIndex = 0;
}

RasPiSPI::~RasPiSPI() { delete[] TxBuffer; }

void RasPiSPI::begin(int channel, int speed)
{
    // Open port for reading and writing
    if (wiringPiSPISetup(channel, speed) < 0)
    {
        cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
    }
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
