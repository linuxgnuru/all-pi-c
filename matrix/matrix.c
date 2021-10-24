#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <wiringPi.h>

const int matrix[3][3] = { { 21, 22, 23 }, { 25, 26, 27 } };

void MyClear();

int main(int argc, char **argv)
{
	int i;
	int j;
	int grid;

	/*
	 * r and c = 0-2
	 *
	 * +-------------------+
	 * |                   |
	 * +----+----+----+----+
	 * |    | 21 | 22 | 23 |
	 * +----+----+----+----+
	 * | 25 |  0 | 1  | 2  |
	 * +----+----+----+----+
	 * | 26 |  3 | 4  | 5  |
	 * +----+----+----+----+
	 * | 27 |  6 | 7  | 8  |
	 * +----+----+----+----+
	 */
	if (argc > 1)
	{
		grid = atoi(argv[1]);
	}
	else
	{
		printf("usage: %s [grid number]\n", argv[0]);
		printf("where grid number is from 0 to 8\n");
		printf("Refer to the following grid:\n");
		printf( "+----+----+----+----+\n"
			"|    | 21 | 22 | 23 |\n"
			"+----+----+----+----+\n"
			"| 25 |  0 | 1  | 2  |\n"
			"+----+----+----+----+\n"
			"| 26 |  3 | 4  | 5  |\n"
			"+----+----+----+----+\n"
			"| 27 |  6 | 7  | 8  |\n"
			"+----+----+----+----+\n");
		return 1;
	}
	if (wiringPiSetup () == -1)
	{
		fprintf(stdout, "oops: %s\n", strerror(errno));
		return 1;
	}
	// set the pins for output
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			pinMode(matrix[i][j], OUTPUT);
	MyClear();
	switch (grid)
	{
		case 0:
			MyClear();
			digitalWrite(21, HIGH);
			digitalWrite(26, HIGH);
			digitalWrite(27, HIGH);
			break;
		case 1:
			MyClear();
			digitalWrite(22, HIGH);
			digitalWrite(26, HIGH);
			digitalWrite(27, HIGH);
			break;
		case 2:
			MyClear();
			digitalWrite(23, HIGH);
			digitalWrite(26, HIGH);
			digitalWrite(27, HIGH);
			break;
		case 3:
			MyClear();
			digitalWrite(21, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(27, HIGH);
			break;
		case 4:
			MyClear();
			digitalWrite(22, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(27, HIGH);
			break;
		case 5:
			MyClear();
			digitalWrite(23, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(27, HIGH);
			break;
		case 6:
			MyClear();
			digitalWrite(21, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(26, HIGH);
			break;
		case 7:
			MyClear();
			digitalWrite(22, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(26, HIGH);
			break;
		case 8:
			MyClear();
			digitalWrite(23, HIGH);
			digitalWrite(25, HIGH);
			digitalWrite(26, HIGH);
			break;
	}
//	digitalWrite(led_pin, (digitalRead(led_pin) == 0 ? HIGH : LOW));
	return 0;
}
/*
bcm2835_gpio_fsel(KEY[i], BCM2835_GPIO_FSEL_INPT);
bcm2835_gpio_set_pud(KEY[i], BCM2835_GPIO_PUD_UP);
//if (bcm2835_gpio_lev(KEY[i]) == 0)        
*/

void MyClear()
{
	int i, j;
	for (i = 0; i < 3; i++)
		for (j = 0; j < 3; j++)
			digitalWrite(matrix[i][j], LOW);
}
