#include <stdio.h>
#include <wiringPi.h>

#define MY_8591 100

int rev(int value) { return (abs(value - 255)); }

int main(int argc, char **argv)
{
	FILE *fp;
	wiringPiSetup();
	pcf8591Setup(MY_8591, 0x48);
	fp = fopen("/dev/shm/light.txt", "w");
	fprintf(fp, "LDR: %0.3d", rev(analogRead(100)));
	fclose(fp);
	return 0;
}
