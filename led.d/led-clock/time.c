#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	struct tm *t;
	time_t tim;

	while (1)
	{
		tim = time(NULL);
		t = localtime(&tim);
		printf("%02d:%02d:%02d\n", t->tm_hour, t->tm_min, t->tm_sec);
		//sprintf(buf, "%02d/%02d/%02d", t->tm_mday, t->tm_mon + 1, t->tm_year-100);
	}
	return 0;
}
