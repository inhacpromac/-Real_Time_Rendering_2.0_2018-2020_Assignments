#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{ 
	char chArray_One[MAX_STRING_LENGTH], chArray_Two[MAX_STRING_LENGTH];

	//***STRING INPUT***
	printf("\n");
	printf("Enter First String : \n");
	gets_s(chArray_One, MAX_STRING_LENGTH);

	printf("\n");
	printf("Enter Second String : \n");
	gets_s(chArray_Two, MAX_STRING_LENGTH);

	//***STRING CONCAT***
	printf("\n");
	printf("****BEFORE CONCATENATION****");
	printf("\n");
	printf("The Original First String Entered By You (i.e : 'chArray_One[]') Is : \n");
	printf("%s\n", chArray_One);

	printf("\n");
	printf("The Original Second String Entered By You (i.e : 'chArray_Two[]') Is : \n");
	printf("%s\n", chArray_Two);

	strcat(chArray_One, chArray_Two);
	strcat(chArray_Two, chArray_One);

	printf("\n");
	printf("****AFTER CONCATENATION****");
	printf("\n");
	printf("'chArray_One[]' Is : \n");
	printf("%s\n", chArray_One);

	printf("\n");
	printf("'chArray_Two[]' Is : \n");
	printf("%s\n", chArray_Two);

	return(0);
}
