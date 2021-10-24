#include <stdio.h>
#include <stdlib.h>
#include <mygps.h>

int main(int argc, char **argv)
{
	loc_t data;

	gps_init();
	while (1)
	{
		gps_location(&data);
		printf("%lf, %lf\n", data.latitude, data.longitude);
	}
	return EXIT_SUCCESS;
}
