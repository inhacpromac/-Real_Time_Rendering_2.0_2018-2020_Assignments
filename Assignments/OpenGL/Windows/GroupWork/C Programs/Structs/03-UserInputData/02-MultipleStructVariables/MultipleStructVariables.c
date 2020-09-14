#include <stdio.h>

struct MyPoint
{
	int x;
	int y;
};

int main(void)
{
	struct MyPoint point_A, point_B, point_C, point_D, point_E;

	//User Input For The Data Members Of 'struct MyPoint' variable 'point_A'
	printf("\n");
	printf("Enter X-Coordinate For Point 'A' : ");
	scanf("%d", &point_A.x);
	printf("Enter Y-Coordinate For Point 'A' : ");
	scanf("%d", &point_A.y);

	//User Input For The Data Members Of 'struct MyPoint' variable 'point_B'
	printf("\n");
	printf("Enter X-Coordinate For Point 'B' : ");
	scanf("%d", &point_B.x);
	printf("Enter Y-Coordinate For Point 'B' : ");
	scanf("%d", &point_B.y);

	//User Input For The Data Members Of 'struct MyPoint' variable 'point_C'
	printf("\n");
	printf("Enter X-Coordinate For Point 'C' : ");
	scanf("%d", &point_C.x);
	printf("Enter Y-Coordinate For Point 'C' : ");
	scanf("%d", &point_C.y);

	//User Input For The Data Members Of 'struct MyPoint' variable 'point_D'
	printf("\n");
	printf("Enter X-Coordinate For Point 'D' : ");
	scanf("%d", &point_D.x);
	printf("Enter Y-Coordinate For Point 'D' : ");
	scanf("%d", &point_D.y);

	//User Input For The Data Members Of 'struct MyPoint' variable 'point_E'
	printf("\n");
	printf("Enter X-Coordinate For Point 'E' : ");
	scanf("%d", &point_E.x);
	printf("Enter Y-Coordinate For Point 'E' : ");
	scanf("%d", &point_E.y);

	//Displaying Values Of The Data Members Of 'struct MyPoint' (all variables)
	printf("\n");
	printf("Co-ordinates (x, y) Of Point 'A' Are : (%d, %d)\n", point_A.x, point_A.y);
	printf("Co-ordinates (x, y) Of Point 'B' Are : (%d, %d)\n", point_B.x, point_B.y);
	printf("Co-ordinates (x, y) Of Point 'C' Are : (%d, %d)\n", point_C.x, point_C.y);
	printf("Co-ordinates (x, y) Of Point 'D' Are : (%d, %d)\n", point_D.x, point_D.y);
	printf("Co-ordinates (x, y) Of Point 'E' Are : (%d, %d)\n", point_E.x, point_E.y);

	return(0);
}
