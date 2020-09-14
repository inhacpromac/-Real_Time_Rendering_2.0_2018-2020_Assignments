// *** THIS PROGRAM REPLACES ALL VOWELS IN THE INPUT STRING WITH THE * (asterisk) SYMBOL ***
// *** FOR EXAMPLE, ORIGINAL STRING 'Dr. Vijay Dattatray Gokhale ASTROMEDICOMP' WILL BECOME 'Dr. V*j*y D*tt*tr*y* G*kh*le *STR*M*D*C*MP' 

#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	int MyStrlen(char[]);
	void MyStrcpy(char[], char[]);

	char chArray_Original[MAX_STRING_LENGTH], chArray_VowelsReplaced[MAX_STRING_LENGTH];
	int iStringLength;
	int i;

	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//***STRING OUTPUT***
	MyStrcpy(chArray_VowelsReplaced, chArray_Original);

	iStringLength = MyStrlen(chArray_VowelsReplaced);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray_VowelsReplaced[i])
		{
		case 'A':
		case 'a':
		case 'E':
		case 'e':
		case 'I':
		case 'i':
		case 'O':
		case 'o':
		case 'U':
		case 'u':
			chArray_VowelsReplaced[i] = '*';
			break;
		default:
			break;
		}
	}

	//***STRING OUTPUT***
	printf("\n");
	printf("String Entered By You Is : \n");
	printf("%s\n", chArray_Original);

	printf("\n");
	printf("String After Replacement Of Vowels By * Is : \n");
	printf("%s\n", chArray_VowelsReplaced);

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

void MyStrcpy(char str_destination[], char str_source[])
{
	int MyStrlen(char[]);

	int iStringLength = 0;
	int j;

	iStringLength = MyStrlen(str_source);
	for (j = 0; j < iStringLength; j++)
		str_destination[j] = str_source[j];

	str_destination[j] = '\0';
}
