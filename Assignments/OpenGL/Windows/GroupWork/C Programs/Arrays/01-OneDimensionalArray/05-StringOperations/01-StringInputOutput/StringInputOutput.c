#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	char chArray[MAX_STRING_LENGTH]; // A Character Array Is A String

	printf("\nEnter A String : \n");
	gets_s(chArray, MAX_STRING_LENGTH);

	printf("\nString Entered By You Is : \n");
	printf("%s\n", chArray);
	return(0);
}

