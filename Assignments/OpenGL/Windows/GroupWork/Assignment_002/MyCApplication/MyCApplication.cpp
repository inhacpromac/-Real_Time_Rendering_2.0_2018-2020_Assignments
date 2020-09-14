// GroupAssignmentV1
// My C Application - Print any character string in 5*5 Matrix

// Header
#include<stdio.h>
#define LIMIT 50

// Function declaration
void funa(int);
void funb(int);
void func(int);
void fund(int);
void fune(int);
void funf(int);
void fung(int);
void funm(int);
void funn(int);
void funr(int);
void funt(int);
void space();
int main()
{
	// Code
	int i, j = 0, id = 0;

	// input
	char cha[LIMIT];
	printf("Enter the String : ");
	gets_s(cha);
	//scanf("%s", cha);
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
		case 'b':
		case 'B':
			funb(i);
			space();
			j++;
			goto next;
		case 'c':
		case 'C':
			func(i);
			space();
			j++;
			goto next;
		case 'd':
		case 'D':
			fund(i);
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

void funb(int i)
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

void func(int i)
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

void fund(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 0 || i == 4) && j > 2)
		{
			printf("  ");
		}
		else if ((i == 1 || i == 3) && (j != 0 || j != 3))
		{
			printf("  ");
		}
		else if ((i == 2) && (j == 0 && j == 4))
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