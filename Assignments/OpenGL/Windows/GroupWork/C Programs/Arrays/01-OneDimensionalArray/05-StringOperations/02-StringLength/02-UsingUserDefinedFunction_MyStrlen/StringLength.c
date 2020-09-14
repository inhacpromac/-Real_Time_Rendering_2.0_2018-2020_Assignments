#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	int MyStrlen(char[]);

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
	iStringLength = MyStrlen(chArray);
	printf("Length Of String Is = %d Characters !!!\n", iStringLength);

	return(0);
}

int MyStrlen(char str[])
{
	int j;
	int string_length = 0;

	//***DETERMINING EXACT LENGTH OF THE STRING, BY DETECTING THE FIRST OCCURENCE OF NULL-TERMINATING CHARACTER ( \0 )***
	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);
}
