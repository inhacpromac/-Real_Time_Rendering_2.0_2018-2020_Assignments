#include <stdio.h>

struct MyData
{
	int i;
	float f;
	double d;
	char c;
} data = { 30, 4.5f, 11.451995, 'A' }; //Inline initialization of struct variable 'data' of type 'struct MyData' 

int main(void)
{
	//Displaying Values Of The Data Members Of 'struct MyData'
	printf("\n");
	printf("DATA MEMBERS OF 'struct MyData' ARE : \n");
	printf("i = %d\n", data.i);
	printf("f = %f\n", data.f);
	printf("d = %lf\n", data.d);
	printf("c = %c\n", data.c);

	return(0);
}
