#include <stdio.h>
#include <string.h> // for strrev()

#define MAX_STRING_LENGTH 256

int main(void)
{
	char chArray_Original[MAX_STRING_LENGTH];


	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	// *** STRING OUTPUT ***
	printf("\n");
	printf("The Original String Entered By You (i.e : 'chArray_Original[]') Is : \n");
	printf("%s\n", chArray_Original);

	printf("\n");
	printf("The Reversed String (i.e : 'chArray_Reverse[]') Is : \n");
	printf("%s\n", strrev(chArray_Original));

	return(0);
}
