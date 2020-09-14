#include <stdio.h>

#define NUM_ELEMENTS 5

int main(void)
{
	int iArray[NUM_ELEMENTS];
	int i, num, sum = 0;

	printf("\n");
	printf("Enter Integer Elememts For Array : \n");
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		scanf("%d", &num);
		iArray[i] = num;
	}

	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		sum = sum + iArray[i];
	}

	printf("\n");
	printf("Sum Of ALL Elements Of Array = %d\n", sum);

	return(0);
}

