#include <stdio.h>

struct MyNumber
{
	int num;
	int num_table[10];
};

struct NumTables
{
	struct MyNumber n;
};

int main(void)
{
	struct NumTables tables[10];
	int i, j;

	for (i = 0; i < 10; i++)
	{
		tables[i].n.num = (i + 1);
	}

	for (i = 0; i < 10; i++)
	{
		printf("\n");
		printf("Table Of %d : \n", tables[i].n.num);
		for (j = 0; j < 10; j++)
		{
			tables[i].n.num_table[j] = tables[i].n.num * (j + 1);
			printf("%d * %d = %d\n", tables[i].n.num, (j + 1), tables[i].n.num_table[j]);
		}
	}
	return(0);
}

