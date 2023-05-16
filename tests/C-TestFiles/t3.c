#include <stdio.h>

main()	
{
	boo();
}

boo() 	/* real main */
{
	int x,y,z;
	x = 2;
	y = 3;
	z = 5;
	foo( x, y, z, z, y, x );
}

foo( int a, int b, int c, int d, int e, int f )
{
	return a + b + c + d + e + f;
}

