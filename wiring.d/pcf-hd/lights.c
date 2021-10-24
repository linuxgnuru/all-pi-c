int main(int argc, char **argv)
{
	int pinN;
	int delayn = 300;

	pinN = 
	if ((fd=wiringPiI2CSetup(devid)) < 0)
	{
	    fprintf(stderr, "Unable to open I2C device: %s\n", strerror (errno));
	    return 1;
	}
				if ((wiringPiI2CWrite(fd,pinN)) < 0)
				{
				    fprintf (stderr, "write badly %s\n", strerror (errno));
				    return 1;
				}
			}
			delay(delayn);
			//usleep(delay);
		}
		swap = (swap == 1 ? 0 : 1);
		if (j++ == 1) x = 2;
	}
	if ((wiringPiI2CWrite(fd,255)) < 0)
	{
	    fprintf (stderr, "write badly %s\n", strerror (errno));
	    return 1;
	}
	return(0);
}
