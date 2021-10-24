#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

int main(int argc, char **argv)
{
	int i, j, k;
	int nums[8];
	int rev_nums[8];
	int pscale;

	j = 0x200;
	for (i = 8; i >= 0; i--)
	{
		j /= 2;
		k = j - 1;
		nums[i] = k;
	}
	for (i = 0; i < 8; i++) rev_nums[i] = nums[i];
	for (i = 0; i < 16; i++)
	{
		if (i < 8) printf("%d\t", nums[i]);
		else
		{
			pscale = abs(i - 16) - 1;
			if (i == 8) printf("255\t");
			else printf("%d\t", rev_nums[pscale]);
		}
		printf("\n");
	}
	return 0;
}
