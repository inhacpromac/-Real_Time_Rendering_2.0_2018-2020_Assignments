// GroupAssignmentV2
// My C Application - Commandline Argument

// Header
#include<stdio.h>
#include<string.h>
#define LIMIT 50

// Function declaration
void funa(int);
void fune(int);
void funf(int);
void fung(int);
void funm(int);
void funn(int);
void funr(int);
void funt(int);
void space();
int main(int argc, char *argv[])
{
	// Code
	int i, j = 0, id = 0;
	
	// input
	char cha[LIMIT];
	strcpy(cha, argv[1]);
	printf("Number of arguments = %d\n", argc);
	printf("Your String : %s\n",cha);
	//printf("%s\n", cha);

	while (cha[j] != '\0')
		j++;

	int max = j;
	//printf("%d\n", max);

	j = 0;

	for (i = 0; i < 5; i++)
	{
	next:
		if (j >= max)
		{
			j = 0;
			goto end;
		}
		switch (cha[j])
		{

		case 'a':
		case 'A':
			funa(i);
			space();
			j++;
			goto next;
		case 'e':
		case 'E':
			fune(i);
			space();
			j++;
			goto next;
		case 'f':
		case 'F':
			funf(i);
			space();
			j++;
			goto next;
		case 'g':
		case 'G':
			fung(i);
			space();
			j++;
			goto next;
		case 'm':
		case 'M':
			funm(i);
			space();
			j++;
			goto next;
		case 'n':
		case 'N':
			funn(i);
			space();
			j++;
			goto next;
		case 'r':
		case 'R':
			funr(i);
			space();
			j++;
			goto next;
		case 't':
		case 'T':
			funt(i);
			space();
			j++;
			goto next;

		}
	end:
		printf("\n");

	}
	
	getchar();
	getchar();
}

void funa(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if (i == 0 && j != 2)
		{
			printf("  ");
		}
		else if (i == 1 && (j == 0 || j == 2 || j == 4))
		{
			printf("  ");
		}
		else if ((i == 3) && (j > 0 && j < 4))
		{
			printf("  ");
		}
		else if ((i == 4) && (j > 0 && j < 4))
		{
			printf("  ");
		}
		else
		{
			printf("* ");
		}
	}
}

void fune(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 1 || i == 3) && j != 0)
		{
			printf("  ");
		}
		else if (i == 2 && (j == 3 || j == 4))
		{
			printf("  ");
		}
		else
			printf("* ");
	}
}

void funf(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 1 || i == 3 || i == 4) && j != 0)
		{
			printf("  ");
		}
		else if (i == 2 && j == 4)
		{
			printf("  ");
		}
		else
		{
			printf("* ");
		}
	}
}

void fung(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if (i == 1 && j != 0)
		{
			printf("  ");
		}
		else if (i == 2 && j == 1)
		{
			printf("  ");
		}
		else if ((i == 3) && (j == 1 || j == 2 || j == 3))
		{
			printf("  ");
		}
		else
		{
			printf("* ");
		}
	}
}

void funm(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 0 || i == 3 || i == 4) && (j == 1 || j == 2 || j == 3))
		{
			printf("  ");
		}
		else if (i == 1 && j == 2)
		{
			printf("  ");
		}
		else if ((i == 2) && (j == 1 || j == 3))
		{
			printf("  ");
		}
		else
		{
			printf("* ");
		}
	}
}

void funn(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 0 || i == 4) && (j == 1 || j == 2 || j == 3))
		{
			printf("  ");
		}
		else if ((i == 2) && (j == 1 || j == 3))
		{
			printf("  ");
		}
		else if ((i == 1) && (j == 2 || j == 3))
		{
			printf("  ");
		}
		else if ((i == 3) && (j == 1 || j == 2))
		{
			printf("  ");
		}
		else
		{
			printf("* ");
		}
	}
}

void funr(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 0 || i == 2) && j == 4)
		{
			printf("  ");
		}
		else if (i == 1 && (j == 1 || j == 2 || j == 3))
		{
			printf("  ");
		}
		else if (i == 3 && (j == 3 || j == 4))
		{
			printf("  ");
		}
		else if ((i == 4) && (j == 1 || j == 2))
		{
			printf("  ");
		}
		else
			printf("* ");
	}
}

void funt(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if (i > 0 && j != 2)
		{
			printf("  ");
		}
		else
		{
			printf("* ");
		}
	}
}

void space()
{
	printf("  ");
}