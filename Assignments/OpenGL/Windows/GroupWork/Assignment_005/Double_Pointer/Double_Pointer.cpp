#include<stdio.h>
#include<stdlib.h>
int main()
{
	int **ptr = NULL;
	int count;
	//ptr can save base address of 5 integer array 
	//ptr is based address of an array of integer pointer

	ptr = (int**)malloc(5 * sizeof(int*));
	//now ptr store 5 int*

	if (ptr == NULL)
	{
		printf("Memory Allocation Failed Exit Now");
		exit(0);
	}

	//memory allocation for column
	for (int i = 0; i < 5; i++)
	{
		ptr[i] = (int*)malloc(3 * sizeof(int));

		if (ptr[i] == NULL)
		{
			printf("Memory Allocation Failed exit now");
			exit(0);
		}
	}

	//accepting elements for 2d array total 15 elemnets 
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			scanf("%d", &ptr[i][j]);
			printf("\n");
		}
	}
	/* for(int i=0;i<5;i++)
	 {
		 for(int j=0;j<3;j++)
		 {
			 scanf("%d",&*(*(ptr+i)+j));
			 printf("\n");
		 }
	 }*/



	 //printing 2d array  elements 

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			printf("%5d", ptr[i][j]);
			printf("\t\t");
		}
		printf("\n\n");
	}
	/*for(int i=0;i<5;i++)
	{
		for(int j=0;j<3;j++)
		{
			   printf("%d",*(*(ptr+i)+j));
			   printf("\t\t");
		 }
		 printf("\n\n");
	}*/

	//printng address of **ptr 
	printf("printng address of **ptr ");
	printf("\n\n");
	for (int i = 0; i < 5; i++)
	{

		printf("%p", (ptr + i));
		printf("\t");
	}
	printf("\n\n");

	//printing address of *ptr
	printf("printing address of *ptr");
	printf("\n\n");
	for (int i = 0; i < 5; i++)
	{

		printf("%p", ptr[i]);
		printf("\n\n");
	}

	printf("checking if these address match with them");
	printf("\n\n");
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			printf("%5p", &(ptr[i][j]));
			printf("\t\t");
		}
		printf("\n\n");
	}
}