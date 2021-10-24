/*
	255 127 63 31 15 7 3 1 0
	254 253 251 247 239 223 191 127
*/
#include <stdio.h>
#include <string.h> 
#include <stdlib.h>

int main(int argc, char **argv)
{
	int max = 0x200;
	int i, j, k, l, m;
	int nums[8];

	for (i = 0; i < 8; i++)
	{
		nums[i] = 1;
	}
	l = j = 0xff;
	k = 0;
	l ^=(1<<k);
	printf("j(255): %d\n", j);
	printf("k: %d\tl: %d\n", k, l);
	j ^=(1<<0);
	printf("0 j(254): %d\n", j);
	j ^=(1<<1);
	printf("1 j(252): %d\n", j);
	j ^=(1<<4);
	printf("4 j(236): %d\n", j);
	/*
	for (i = 0; i < 8; i++)
	{
		printf("i: %d\tnums: %d\n", i, nums[i]);
	}
	*/
	return 0;
}
