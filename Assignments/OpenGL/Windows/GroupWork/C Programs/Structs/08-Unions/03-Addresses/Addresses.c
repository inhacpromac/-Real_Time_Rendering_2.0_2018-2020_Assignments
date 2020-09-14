#include <stdio.h>

struct MyStruct
{
	int i;
	float f;
	double d;
	char c;
};

union MyUnion
{
	int i;
	float f;
	double d;
	char c;
};

int main(void)
{
	struct MyStruct s;
	union MyUnion u;

	printf("\n");
	printf("Members Of Struct Are : \n");

	s.i = 9;
	s.f = 8.7f;
	s.d = 1.233422;
	s.c = 'P';

	printf("s.i = %d\n", s.i);
	printf("s.f = %f\n", s.f);
	printf("s.d = %lf\n", s.d);
	printf("s.c = %c\n", s.c);


	printf("Addresses Of Members Of Struct Are : \n");
	printf("s.i = %p\n", &s.i);
	printf("s.f = %p\n", &s.f);
	printf("s.d = %p\n", &s.d);
	printf("s.c = %p\n", &s.c);

	printf("MyStruct s = %p\n", &s);

	printf("\n");
	printf("Members Of Union Are : \n");
	
	u.i = 3;
	printf("u.i = %d\n", u.i);

	u.f = 4.5f;
	printf("u.f = %f\n", u.f);

	u.d = 5.12764;
	printf("u.d = %lf\n", u.d);

	u.c = 'A';
	printf("u.c = %c\n", u.c);


	printf("Addresses Of Members Of Union Are : \n");
	printf("u.i = %p\n", &u.i);
	printf("u.f = %p\n", &u.f);
	printf("u.d = %p\n", &u.d);
	printf("u.c = %p\n", &u.c);

	printf("MyUnion u = %p\n", &u);

	return(0);
}
