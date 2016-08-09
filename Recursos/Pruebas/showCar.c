#include <stdio.h>
int main()
{
	int i=0;
	for(i=0;i<256;i++)
		printf("[%d->(%c)] ",i, (char)i);


	printf("\n\n\n---->%d", '·');

	return 0;
}
