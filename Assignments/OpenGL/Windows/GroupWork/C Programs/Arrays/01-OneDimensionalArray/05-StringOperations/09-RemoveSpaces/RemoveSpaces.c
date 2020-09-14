#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	char chArray[MAX_STRING_LENGTH], chArray_SpacesRemoved[MAX_STRING_LENGTH];
	int iStringLength;
	int i, j;

	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray, MAX_STRING_LENGTH);

	iStringLength = MyStrlen(chArray);
	j = 0;
	for (i = 0; i < iStringLength; i++)
	{
		if (chArray[i] == ' ')
			continue;
		else
		{
			chArray_SpacesRemoved[j] = chArray[i];
			j++;
		}
	}

	chArray_SpacesRemoved[j] = '\0';

	//***STRING OUTPUT***
	printf("\n");
	printf("String Entered By You Is : \n");
	printf("%s\n", chArray);

	printf("\n");
	printf("String After Removal Of Spaces Is : \n");
	printf("%s\n", chArray_SpacesRemoved);

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
