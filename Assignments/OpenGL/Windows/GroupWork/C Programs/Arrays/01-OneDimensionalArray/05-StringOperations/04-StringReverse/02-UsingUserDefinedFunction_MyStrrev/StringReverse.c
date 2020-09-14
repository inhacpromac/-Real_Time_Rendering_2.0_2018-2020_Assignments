#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	void MyStrrev(char[], char[]);

	char chArray_Original[MAX_STRING_LENGTH], chArray_Reversed[MAX_STRING_LENGTH];

	//***STRING INPU***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray_Original, MAX_STRING_LENGTH);

	//***STRING REVERSE***
	MyStrrev(chArray_Reversed, chArray_Original);

	//***STRING OUTPUT***
	printf("\n");
	printf("The Original String Entered By You (i.e : 'chArray_Original[]') Is : \n");
	printf("%s\n", chArray_Original);

	printf("\n");
	printf("The Reversed String (i.e : 'chArray_Reversed[]') Is : \n");
	printf("%s\n", chArray_Reversed);

	return(0);
}

void MyStrrev(char str_destination[], char str_source[])
{
	int MyStrlen(char[]);

	int iStringLength = 0;
	int i, j, len;

	iStringLength = MyStrlen(str_source);

	//ARRAY INDICES BEGIN FROM 0, HENCE, LAST INDEX WILL ALWAYS BE (LENGTH - 1)
	len = iStringLength - 1;

	//WE NEED TO PUT THE CHARACTER WHICH IS AT LAST INDEX OF 'str_source' TO THE FIRST INDEX OF 'str_destination'
	//AND SECOND-LAST CHARACTER OF 'str_source' TO THE SECOND CHARACTER OF 'str_destination' and so on...
	for (i = 0, j = len; i < iStringLength, j >= 0; i++, j--)
	{
		str_destination[i] = str_source[j];
	}

	str_destination[i] = '\0';
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
