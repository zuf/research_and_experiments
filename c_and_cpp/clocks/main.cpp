#include <stdlib.h>
#include <stdio.h>
#include <time.h>

int main(int argc, char **argv)
{
	printf("Compiled at %s %s\n\n", __DATE__, __TIME__);

	printf("clock(): %d\n", (int)clock());
	printf("CLOCKS_PER_SEC: %d\n", (int)CLOCKS_PER_SEC);
	printf("Time: lock()/CLOCKS_PER_SEC: %f\n", (double)clock()/CLOCKS_PER_SEC);
	return 0;
}
