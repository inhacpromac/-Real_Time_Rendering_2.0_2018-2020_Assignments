#include <stdio.h>

struct MyPoint
{
	int x;
	int y;
};

struct Rectangle
{
	struct MyPoint point_01, point_02;
};

int main(void)
{
	int length, breadth, area;
	struct Rectangle rect;

	printf("\n");
	printf("Enter Leftmost X-Coordinate Of Rectangle : ");
	scanf("%d", &rect.point_01.x);

	printf("\n");
	printf("Enter Bottommost Y-Coordinate Of Rectangle : ");
	scanf("%d", &rect.point_01.y);

	printf("\n");
	printf("Enter Rightmost X-Coordinate Of Rectangle : ");
	scanf("%d", &rect.point_02.x);

	printf("\n");
	printf("Enter Topmost Y-Coordinate Of Rectangle : ");
	scanf("%d", &rect.point_02.y);

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
