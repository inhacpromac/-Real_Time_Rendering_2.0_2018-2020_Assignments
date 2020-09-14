MultiColored Concentric Circles

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
#define _USE_MATH_DEFINES 1
#include<math.h>
#define num_point 1000

void drawCircle(GLfloat, int);

//Display
GLfloat angle = 0.0f;
GLfloat i = 0.1f;
int j = 0;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
		
	for (i = 0.1f, j = 1; i < 0.9f || j < 9; i += 0.1f, j++)
	{
		drawCircle(i, j);
	}
	
	SwapBuffers(ghdc);
}

void drawCircle(GLfloat i, int j)
{
	glBegin(GL_LINE_LOOP);
	if (j == 1)
	{
		glColor3f(0.5f, 0.5f, 0.5f);
	}
	else if (j == 2)
	{
		glColor3f(1.0f, 0.5f, 0.5f);
	}
	else if (j == 3)
	{
		glColor3f(1.0f, 0.0f, 1.0f);
	}
	else if (j == 4)
	{
		glColor3f(0.0f, 1.0f, 1.0f);
	}
	else if (j == 5)
	{
		glColor3f(1.0f, 1.0f, 0.0f);
	}
	else if (j == 6)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	else if (j == 7)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
	}
	else
	{
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	
	for (angle = 0.0f; angle < 2 * M_PI; angle += 0.01f)
	{
		glVertex3f((GLfloat)cos(angle) * i, (GLfloat)sin(angle) * i, 0.0f);
	}

	glEnd();
}

//End