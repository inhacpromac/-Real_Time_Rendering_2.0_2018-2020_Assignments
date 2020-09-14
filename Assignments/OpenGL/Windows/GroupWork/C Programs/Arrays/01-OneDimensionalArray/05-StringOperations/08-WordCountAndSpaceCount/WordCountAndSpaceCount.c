#include <stdio.h>

#define MAX_STRING_LENGTH 512

int main(void)
{
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	char chArray[MAX_STRING_LENGTH];
	int iStringLength;
	int i;
	int word_count = 0, space_count = 0;

	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray, MAX_STRING_LENGTH);

	iStringLength = MyStrlen(chArray);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray[i])
		{
		case 32: //32 IS THE ASCII VALUE FOR SPACE (' ') CHARACTER 
			space_count++;
			break;
		default:
			break;
		}
	}

	word_count = space_count + 1;

	//**STRING OUTPUT***
	printf("\n");
	printf("String Entered By You Is : \n");
	printf("%s\n", chArray);

	printf("\n");
	printf("Number Of Spaces In The Input String = %d\n", space_count);
	printf("Number Of Words In The Input String = %d\n", word_count);

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
