#include <stdio.h>

main()	
{
	boo();
}

boo() 	/* real main */
{
	int x,y;
	scanf( "%d", &x );
	y = x*x - 2*x + 1;
	printf( "%d", y );
}

