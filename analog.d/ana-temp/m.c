#include <stdio.h>
#include <string.h>
#include <math.h>

double Therm(int data)
{
	double Temp;

	Temp = log(((10240000/data) - 10000));
	Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp)) * Temp);
	Temp = Temp - 273.15; // convert Kelvin to Celcius
	return Temp;
}

int main(int argc, char **argv)
{
	int num;
	double Temp;

	if (argc != 2)
	{
		printf("usage: %s number\n", argv[0]);
		return 1;
	}
	num = atoi(argv[1]);
	Temp = Therm(num);
	printf("temp: %f\n", Temp);
	return 0;
}
