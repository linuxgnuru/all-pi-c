#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//#define PIN RPI_GPIO_P1_12
#define PIN RPI_GPIO_P1_16

#define TRUE 1
#define FALSE 0

int main(int argc, char **argv)
{
	char c_result[2];
	char turn;
	char n, p, result;
	unsigned long i;
	unsigned long buff[33];
	char key[5];
	int ctr;
	int r_flag;
	int debug_flag;
	int result_int;

	if (!bcm2835_init())
		return 1;
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
	printf("irm test start: \n");
	debug_flag = FALSE;
	r_flag = ((argc > 1) && ((strcmp(argv[1], "mp3") == 0)) ? 2 : 1);
	ctr = 1;
	while (1)
	{
		i++; // #i#
		delayMicroseconds(60);
		if (bcm2835_gpio_lev(PIN) == 1)
			turn = 1;
		else
		{
			if (turn == 1)
			{
				if ((n > 0) && (n <= 33))
					buff[n-1] = i; // #i#
				n++;
				i = 0; // #i#
				if (n == 34)
				{
					n = 0;
					/*
					for (p = 0; p < 33; p++)
						printf("%d-%d;", p, buff[p]);
					*/
					if (buff[0] > 180 && buff[0] < 250 && buff[1] < 25 && buff[2] < 25 && buff[3] < 25 && buff[4] < 25 && buff[5] < 25 && buff[6] < 25 &&
						buff[7] < 25 && buff[8] < 25 && buff[9] > 25 && buff[10] > 25 && buff[11] > 25 && buff[12] > 25 && buff[13] > 25)
					{
						for (p = 0; p < 8; p++)
						{
							result >>= 1;
							if (buff[25+p] > 25)
								result |= 0x80;
						}
						if (r_flag == 1)
						{

							switch (result)
							{
								case 186: strcpy(key, "CH- "); break; // 186
								case 185: strcpy(key, "CH  "); break; // 185
								case 184: strcpy(key, "CH+ "); break; // 184
								case 187: strcpy(key, "Prev"); break; // 187
								case 191: strcpy(key, "Next"); break; // 191
								case 188: strcpy(key, "Play"); break; // 188
								case 248: strcpy(key, "Vol-"); break; // 248
								case 234: strcpy(key, "Vol+"); break; // 234 *
								case 246: strcpy(key, "Equi"); break; // 246
								case 233: strcpy(key, "0   "); break; // 233 *
								case 230: strcpy(key, "100+"); break; // 230 *
								case 242: strcpy(key, "200+"); break; // 242 *
								case 243: strcpy(key, "1   "); break; // 243
								case 231: strcpy(key, "2   "); break; // 231 *
								case 161: strcpy(key, "3   "); break; // 161
								case 247: strcpy(key, "4   "); break; // 247
								case 227: strcpy(key, "5   "); break; // 227 *
								case 165: strcpy(key, "6   "); break; // 165
								case 189: strcpy(key, "7   "); break; // 189
								case 173: strcpy(key, "8   "); break; // 173
								case 181: strcpy(key, "9   "); break; // 181
								default: sprintf(key, "0x%x", result); //strcpy(key, " "); break; // 
								//default: strcpy(key, "N/A "); break; // Shouldn't get here.
							}
						}
						if (r_flag == 2)
						{
							switch (result)
							{
								case 0xa5: strcpy(key, "5   "); break; // 
								case 0xad: strcpy(key, "7   "); break; // 
								case 0xb5: strcpy(key, "8   "); break; // 
								case 0xb8: strcpy(key, "Mute"); break; // 
								case 0xb9: strcpy(key, "Mode"); break; // 
								case 0xba: strcpy(key, "Powr"); break; // 
								case 0xbb: strcpy(key, "Play"); break; // 
								case 0xbc: strcpy(key, "Prev"); break; // 
								case 0xbd: strcpy(key, "6   "); break; // 
								case 0xbf: strcpy(key, "Next"); break; // 
								case 0xe6: strcpy(key, "Rept"); break; // 
								case 0xe7: strcpy(key, "3   "); break; // 
								case 0xe3: strcpy(key, "4   "); break; // 
								case 0xe9: strcpy(key, "0   "); break; // 
								case 0xea: strcpy(key, "Vol-"); break; // 
								case 0xf2: strcpy(key, "U/SD"); break; // 
								case 0xf3: strcpy(key, "1   "); break; // 
								case 0xf6: strcpy(key, "Vol+"); break; // 
//								case 0xf6: strcpy(key, "2   "); break; // 
								case 0xf8: strcpy(key, "Eqlz"); break; // 
								case 0xf7: strcpy(key, "9   "); break; // 
								default: sprintf(key, "0x%x", result); //strcpy(key, " "); break; // 
							} // end switch
						} // end r_flag == 2 if
						if (debug_flag == TRUE)
							fprintf(stderr, "%d %s ox%x \n", ctr, key, result);
						//printf("%d %s ox%x \n", ctr, key, result);
						//printf("ctr\tkey\t[result]\n");
						printf("result (int) %d\n", result);
						printf("%d\t%s\t[%x]\n", ctr, key, result);
					}
					bcm2835_delay(200);
					ctr++;
				}
			}
			turn = 0;
		}
	}
	bcm2835_close();
	printf("ctr: %d\n", ctr-1);
	return 0;
}

