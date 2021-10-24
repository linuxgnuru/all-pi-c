#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(int argc, char **argv)
{
	int i;

	for (i = 98; i < 110; i++)
	{
		printf("%0.3d\n", i);
	}
	return 0;
}
