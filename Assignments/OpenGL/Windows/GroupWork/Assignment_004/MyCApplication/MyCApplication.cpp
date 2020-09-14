// GroupAssignmentV3
// MyCApplication - File I/O

// Header
#include<stdio.h>
#define LIMIT 50

FILE *file = NULL;

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
	// File I/O
	if (fopen_s(&file, "OutPut.txt", "w") != 0)
	{
		fprintf(file, "Log File Cannot be Created\n");
	}
	else
	{
		fprintf(file, "Log File Successfully Created\n");
		fprintf(file, "By Rohan\n");
	}

	// Code
	int i = 0, j = 0, id = 0;

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
	
	for (int i = 0; i < 5; i++)
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
		fprintf(file, "\n");

	}
	//getchar();
}

void funa(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if (i == 0 && j != 2)
		{
			fprintf(file, "  ");
		}
		else if (i == 1 && (j == 0 || j == 2 || j == 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 3) && (j > 0 && j < 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 4) && (j > 0 && j < 4))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if (i == 1 && (j == 0 || j == 2 || j == 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 3) && (j > 0 && j < 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 4) && (j > 0 && j < 4))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if (i == 1 && (j == 0 || j == 2 || j == 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 3) && (j > 0 && j < 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 4) && (j > 0 && j < 4))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if ((i == 1 || i == 3) && (j != 0 || j != 3))
		{
			fprintf(file, "  ");
		}
		else if ((i == 2) && (j == 0 && j == 4))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if (i == 2 && (j == 3 || j == 4))
		{
			fprintf(file, "  ");
		}
		else
			fprintf(file, "* ");
	}
}

void funf(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if ((i == 1 || i == 3 || i == 4) && j != 0)
		{
			fprintf(file, "  ");
		}
		else if (i == 2 && j == 4)
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if (i == 2 && j == 1)
		{
			fprintf(file, "  ");
		}
		else if ((i == 3) && (j == 1 || j == 2 || j == 3))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if (i == 1 && j == 2)
		{
			fprintf(file, "  ");
		}
		else if ((i == 2) && (j == 1 || j == 3))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if ((i == 2) && (j == 1 || j == 3))
		{
			fprintf(file, "  ");
		}
		else if ((i == 1) && (j == 2 || j == 3))
		{
			fprintf(file, "  ");
		}
		else if ((i == 3) && (j == 1 || j == 2))
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
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
			fprintf(file, "  ");
		}
		else if (i == 1 && (j == 1 || j == 2 || j == 3))
		{
			fprintf(file, "  ");
		}
		else if (i == 3 && (j == 3 || j == 4))
		{
			fprintf(file, "  ");
		}
		else if ((i == 4) && (j == 1 || j == 2))
		{
			fprintf(file, "  ");
		}
		else
			fprintf(file, "* ");
	}
}

void funt(int i)
{
	int j;
	for (j = 0; j < 5; j++)
	{
		if (i > 0 && j != 2)
		{
			fprintf(file, "  ");
		}
		else
		{
			fprintf(file, "* ");
		}
	}
}

void space()
{
	fprintf(file, "  ");
}