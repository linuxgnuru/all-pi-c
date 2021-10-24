#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define die(str, args...) do { \
	perror(str); \
	exit(EXIT_FAILURE); \
} while (0)

int main(int argc, char **argv)
{
	char buf[80];
	//printf("%s, %d\n", __FILE__, __LINE__);
	sprintf(buf, "[%d, %s] error: write", __LINE__, __FILE__ + 1);
	die(buf);
	return 0;
}
