#ifndef __RASPISPI_H
#define __RASPISPI_H

#define CE 0

class RasPiSPI
{
    private:
        unsigned char *TxBuffer;
        int TxBufferIndex;

    public:
        RasPiSPI();
        ~RasPiSPI();

        void begin() { begin(CE, 1000000); } // default use channel 0 and 1MHz clock speed
        void begin(int, int);

        void transfer(char);
        void endTransfer();
};

#endif
