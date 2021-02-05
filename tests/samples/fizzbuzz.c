/*
 * From 0 to n-1, print out "Fizz"
 * if the index is divisible by 3,
 * "Buzz" if it's divisible by 5,
 * and "Fizz Buzz" if it's divisible by 3 and 5.
 * Otherwise, print out the number itself*/

#include <stdio.h>
int main()
{
	for(unsigned i=0;i<=100;i++)
	{
		if(!(i%15))
		{
			printf("Fizz Buzz\n");
		}
		else if(!(i%3))
		{
			printf("Fizz\n");
		}
		else if(!(i%5))
		{
			printf("Buzz\n");
		}
		else
		{
			printf("%d\n", i);
		}
	}
}
