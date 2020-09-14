#include <stdio.h>

#define NUM_ELEMENTS 5

int main(void)
{
	int iArray[NUM_ELEMENTS];
	int i, num, j, count = 0;

	printf("\n");

	//***ARRAY ELEMENTS INPUT***
	printf("Enter Integer Elememts For Array : \n");
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		scanf("%d", &num);

		// If 'num' is negative ( < 0 ), then convert it to positive (multiply by -1)
		if (num < 0)
			num = -1 * num;

		iArray[i] = num;
	}

	//***PRINTING ENTIRE ARRAY***
	printf("\n");
	printf("Array Elements Are : \n");
	for (i = 0; i < NUM_ELEMENTS; i++)
		printf("%d\n", iArray[i]);

	//***SEPARATING OUT EVEN NUMBERS FROM ARRAY ELEMENTS***
	printf("\n");
	printf("Prime Numbers Amongst The Array Elements Are : \n");
	for (i = 0; i < NUM_ELEMENTS; i++)
	{
		for (j = 1; j <= iArray[i]; j++)
		{
			if ((iArray[i] % j) == 0)
				count++;
		}

		// NUMBER 1 IS NEITHER A PRIME NUMBER NOR A CONSONANT
		// IF A NUMBER IS PRIME, IT IS ONLY DIVISIBLE BY 1 AND ITSELF.
		// HENCE, IF A NUMBER IS PRIME, THE VALUE OF 'count' WILL BE EXACTLY 2.
		// IF THE VALUE OF 'count' IS GREATER THAN 2, THE NUMBER IS DIVISIBLE BY NUMBERS OTHER THAN 1 AND ITSLEF AND HENCE, IT IS NOT PRIME
		// THE VALUE OF 'count' WILL BE 1 ONLY IF iArray[i] IS 1.
		if (count == 2)
			printf("%d\n", iArray[i]);

		count = 0; //RESET 'count' TO 0 FOR CHECKING THE NEXT NUMBER...
	}

	return(0);
}

