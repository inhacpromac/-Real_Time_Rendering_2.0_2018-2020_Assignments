#include <stdio.h>

int main(void)
{
	int length, breadth, area;

	struct Rectangle
	{
		struct MyPoint
		{
			int x;
			int y;
		} point_01, point_02;

	} rect = { { 2, 3 },{ 5, 6 } };

	length = rect.point_02.y - rect.point_01.y;
	if (length < 0)
		length = length * -1;

	breadth = rect.point_02.x - rect.point_01.x;
	if (breadth < 0)
		breadth = breadth * -1;

	area = length * breadth;

	printf("\n");
	printf("Length Of Rectangle = %d\n", length);
	printf("Breadth Of Rectangle = %d\n", breadth);
	printf("Area Of Rectangle = %d\n", area);

	return(0);
}
