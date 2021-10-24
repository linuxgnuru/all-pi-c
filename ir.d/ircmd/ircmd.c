#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

//#include<wiringPi.h>
//#include<lcd.h>

#define PIN RPI_GPIO_P1_12

#define TRUE 1
#define FALSE 0


/*
// the following is for LCD display
const int RS = 3;	//
const int EN = 14;	//
const int D0 = 4;	//
const int D1 = 12;	//
const int D2 = 13;	//
const int D3 = 6;	//
*/

int main(int argc, char **argv)
{
	char c_result[2];
	FILE *fp;
	char path[1035];

	if (!bcm2835_init())
		return 1;
	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(PIN, BCM2835_GPIO_PUD_UP);
//	printf("irm test start: \n");
	char turn;
	char n, p, result;
	unsigned long i;
	unsigned long buff[33];
	char key[5];
	//int ctr;
	//int r_flag;
	//int debug_flag;

	if ((argc > 1) && ((strcmp(argv[1], "help") == 0)))
	{
		printf("%s\n");
		printf("ls\tCH-\n");
		printf("du -sh *\tCH\n");
		printf("df -h\tCH+\n");
		printf("reboot\tPrev\n");
		printf("halt\tNext\n");
		printf("startx\tPlay\n");
		printf("amixer sset 'Master' 10% -\tvol-\n");
		printf("amixer sset 'Master' 10% +\tvol+\n");
		printf("alsamixer\tEqui\n");
		printf("sudoku\t0\n");
		printf("top\t100+\n");
		printf("htop\t200+\n");
		printf("mount\t1\n");
		printf("ifconfig\t2\n");
		printf("lsusb\t3 \n");
		printf("i2cdetect -y 1\t4\n");
		printf("gpio readall\t5\n");
		printf("w\t6\n");
		printf("ps ax\t7\n");
		printf("python\t8\n");
		printf("logoug\t9\n");
		return 1;
		//debug_flag = FALSE;
		/*
		if ((argc > 1) && ((strcmp(argv[1], "mp3") == 0))) r_flag = 2;
		else r_flag = 1;
		ctr = 1;
		*/
	}
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
				{
					buff[n-1] = i; // #i#
				}
				n++;
				i = 0; // #i#
				if (n == 34)
				{
					n = 0;
					/*
					for (p = 0; p < 33; p++)
					{
						printf("%d-%d;", p, buff[p]);
					}
					*/
					if (buff[0] > 180 && buff[0] < 250 && buff[1] < 25 && buff[2] < 25 && buff[3] < 25 && buff[4] < 25 && buff[5] < 25 && buff[6] < 25 &&
						buff[7] < 25 && buff[8] < 25 && buff[9] > 25 && buff[10] > 25 && buff[11] > 25 && buff[12] > 25 && buff[13] > 25)
					{
						for (p = 0; p < 8; p++)
						{
							result >>= 1;
							if (buff[25+p] > 25)
							{
								result |= 0x80;
							}
						}
						switch (result)
						{
							//case 186: system("ls"); break; // 186 CH-
							case 186:
								fp = popen("/bin/ls", "r");
								if (fp == NULL)
								{
									printf("Failed to run command\n");
									exit;
								} 
								while (fgets(path, sizeof(path)-1, fp) != NULL)
								{
									printf("%s", path);
								}
								pclose(fp);
								break; // 186 CH-
							case 185: system("du -sh *"); break; // 185 CH
							case 184: system("df -h"); break; // 184 CH+
							case 187: system("reboot"); break; // 187 Prev
							case 191: system("halt"); break; // 191 Next
							case 188: system("startx"); break; // 188 Play
							case 248: system("amixer sset 'Master' 10% -"); break; // 248 vol-
							case 234: system("amixer sset 'Master' 10% +"); break; // 234 * vol+
							case 246: system("alsamixer"); break; // 246 Equi
							case 233: system("sudoku"); break;; // 233 * 0
							case 230: system("top"); break;; // 230 * 100+
							case 242: system("htop"); break;; // 242 * 200+
							case 243: system("mount"); break;; // 243 1
							case 231: system("ifconfig"); break;; // 231 * 2
							case 161: system("lsusb"); break;; // 161 3 
							case 247: system("i2cdetect -y 1"); break;; // 247 4
							case 227: system("gpio readall"); break;; // 227 * 5
							case 165: system("w"); break;; // 165 6
							case 189: system("ps ax"); break; // 189 7
							case 173: system("python"); break; // 173 8
							case 181: system("logout"); break; // 181 9
							default: break;
							//default: sprintf(key, "0x%x", result); //strcpy(key, " "); break; // 
							//default: strcpy(key, "N/A "); break; // Shouldn't get here.
						}
					}
//					if (debug_flag == TRUE) fprintf(stderr, "%d %s ox%x \n", ctr, key, result);
					//printf("%d %s ox%x \n", ctr, key, result);
					//printf("ctr\tkey\t[result]\n");
					//printf("result (int) %d\n", result);
					//printf("%d\t%s\t[%x]\n", ctr, key, result);
				}
				bcm2835_delay(200);
//				ctr++;
			}
		}
		turn = 0;
	}
	bcm2835_close();
//	printf("ctr: %d\n", ctr-1);
	return 0;
}
