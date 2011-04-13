#include <stdlib.h>
#include <stdio.h>

struct fff
{
	unsigned int f:2;
	unsigned int g:1;
	unsigned int h:1;
	unsigned int:0;
	unsigned int:8;
	unsigned int:0;
	unsigned int:1;
} flags;

int main(int argc, char **argv)
{
	printf("Compiled at %s %s\n\n", __DATE__, __TIME__);
	flags.f=2;
	flags.h=true;
	printf("Bit fields: %d %d %d\nsizof(flags): %d\n", flags.f, flags.g, flags.h, (int)sizeof(flags));
	printf("sizof(int): %d\n", (int)sizeof(int));
	return 0;
}
