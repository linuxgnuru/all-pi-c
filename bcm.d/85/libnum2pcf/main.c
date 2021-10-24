#include <stdio.h>
#include <string.h> 

#include "num2pcf.h"

int main(int argc, char **argv)
{
	int devid = 0x20;
	int user_pin;

	if (argc > 1)
	{
		user_pin = atoi(argv[1]);
		if (argc == 3)
		{
			sscanf(argv[2], "%x", &devid);
			// if the address is too low or high; just pick a default.
			if (devid > 0x27 || devid < 0x20)
			{
				printf("bad devid: %x\n", devid);
				devid = 0x20;
			}
		}
	}
	else
	{
		printf("Usage: %s [PIN NUMBER] [ADDRESS]\n", argv[0]);
		printf("\tWhere PIN NUMBER is the pin you wish to set and\n");
		printf("\tthe ADDRESS is the I2C address; if not set a default of 0x20 will be used.\n");
		printf("\tNOTE: ADDRESS must be in hexadecimal format; i.e. 0x20\n");
		return 1;
	}
	if (num2pcf(devid, user_pin))
	{
	    fprintf(stderr, "Error: \n");
	    return 1;
	}
	return 0;
}
