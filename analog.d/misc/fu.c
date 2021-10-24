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

	printf("254 253 251 247 239 223 191 127\n");
	j = max;
	l = 256;
	k = 1;
	for (i = 0; i < 8; i++)
	{
		m = l - k;
		k *= 2;
		nums[i] = m;
	}
	for (i = 0; i < 8; i++)
		printf("i: %d\tnums: %d\n", i, nums[i]);
	return 0;
}
