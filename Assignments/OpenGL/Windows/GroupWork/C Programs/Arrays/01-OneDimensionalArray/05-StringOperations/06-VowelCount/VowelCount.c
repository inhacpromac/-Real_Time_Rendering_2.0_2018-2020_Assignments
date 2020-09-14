#include <stdio.h>

#define MAX_STRING_LENGTH 256

int main(void)
{
	int MyStrlen(char[]);
 
	char chArray[MAX_STRING_LENGTH];
	int iStringLength;
	int count_A = 0, count_E = 0, count_I = 0, count_O = 0, count_U = 0; // Initial Counts = 0
	int i;

	//***STRING INPUT***
	printf("\n");
	printf("Enter A String : \n");
	gets_s(chArray, MAX_STRING_LENGTH);

	//***STRING OUTPUT***
	printf("\n");
	printf("String Entered By You Is : \n");
	printf("%s\n", chArray);

	iStringLength = MyStrlen(chArray);

	for (i = 0; i < iStringLength; i++)
	{
		switch (chArray[i])
		{
		case 'A':
		case 'a':
			count_A++;
			break;
		case 'E':
		case 'e':
			count_E++;
			break;
		case 'I':
		case 'i':
			count_I++;
			break;
		case 'O':
		case 'o':
			count_O++;
			break;
		case 'U':
		case 'u':
			count_U++;
			break;
		default:
			break;
		}
	}

	printf("\n");
	printf("In The String Entered By You, The Vowels And The Number Of Their Occurences Are Are Follows : \n");
	printf("'A' Has Occured = %d Times !!!\n", count_A);
	printf("'E' Has Occured = %d Times !!!\n", count_E);
	printf("'I' Has Occured = %d Times !!!\n", count_I);
	printf("'O' Has Occured = %d Times !!!\n", count_O);
	printf("'U' Has Occured = %d Times !!!\n", count_U);

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
