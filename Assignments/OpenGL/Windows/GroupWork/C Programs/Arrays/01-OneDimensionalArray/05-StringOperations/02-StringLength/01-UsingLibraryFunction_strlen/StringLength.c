#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{ 
	char chArray[MAX_STRING_LENGTH]; //Character Array Is A String
	int iStringLength = 0;

	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray, MAX_STRING_LENGTH);

	//***STRING OUTPUT***
	printf("\n");
	printf("String Entered By You Is : \n");
	printf("%s\n", chArray);

	//***STRING LENGTH***
	printf("\n");
	iStringLength = strlen(chArray);
	printf("Length Of String Is = %d Characters !!!\n", iStringLength);

	return(0);
}
