#include <stdio.h>
#include <ctype.h> //for toupper()

#define MAX_STRING_LENGTH 256

int main(void)
{
	int MyStrlen(char[]);

	char chArray[MAX_STRING_LENGTH], chArray_CapitalizedFirstLetterOfEveryWord[MAX_STRING_LENGTH];
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
		if (i == 0)
			chArray_CapitalizedFirstLetterOfEveryWord[j] = toupper(chArray[i]);

		else if (chArray[i] == ' ')
		{
			chArray_CapitalizedFirstLetterOfEveryWord[j] = chArray[i];
			chArray_CapitalizedFirstLetterOfEveryWord[j + 1] = toupper(chArray[i + 1]);

			//SINCE, ALREADY TWO CHARACTERS (AT INDICES 'i' 
			//AND i + 1 HAVE BEEN CONSIDERED IN THIS else-if BLOCK...WE ARE EXTRA-INCREMENTING 'i' AND 'j' BY 1
			j++;
			i++;
		}

		else
			chArray_CapitalizedFirstLetterOfEveryWord[j] = chArray[i];

		j++;
	}

	chArray_CapitalizedFirstLetterOfEveryWord[j] = '\0';
	
		//***STRING OUTPUT***
	printf("\n");
	printf("String Entered By You Is : \n");
	printf("%s\n", chArray);

	printf("\n");
	printf("String After Capitalizing First Letter Of Every Word : \n");
	printf("%s\n", chArray_CapitalizedFirstLetterOfEveryWord);

	return(0);
}

int MyStrlen(char str[])
{
	int j;
	int string_length = 0;

	// *** DETERMINING EXACT LENGTH OF THE STRING, BY DETECTING THE FIRST OCCURENCE OF NULL-TERMINATING CHARACTER ( \0 ) ***
	for (j = 0; j < MAX_STRING_LENGTH; j++)
	{
		if (str[j] == '\0')
			break;
		else
			string_length++;
	}
	return(string_length);
}
