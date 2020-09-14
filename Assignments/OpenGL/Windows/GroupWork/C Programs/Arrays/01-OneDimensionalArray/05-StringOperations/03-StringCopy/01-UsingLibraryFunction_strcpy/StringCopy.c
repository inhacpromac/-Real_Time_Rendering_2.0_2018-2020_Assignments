#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	char chArray_Original[MAX_STRING_LENGTH];
	char chArray_Copy[MAX_STRING_LENGTH];


	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//***STRING COPY***
	strcpy(chArray_Copy, chArray_Original);

	//***STRING OUTPUT***
	printf("\n");
	printf("The Original String Entered By You (i.e : 'chArray_Original[]') Is : \n");
	printf("%s\n", chArray_Original);

	printf("\n");
	printf("The Copied String (i.e : 'chArray_Copy[]') Is : \n");
	printf("%s\n", chArray_Copy);

	return(0);
}
