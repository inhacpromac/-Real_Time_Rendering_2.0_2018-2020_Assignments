MultiColored Concentric Triangles

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
#define _USE_MATH_DEFINES 1
#include<math.h>

void drawTriangle(GLfloat);

//Display
GLfloat i = 0.0f;
   
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	for (i = 0.0f; i < 0.8f; i += 0.1f)
	{
		drawTriangle(i);
	}
	
	SwapBuffers(ghdc);
}

void drawTriangle(GLfloat i)
{
	glBegin(GL_LINE_LOOP);
	if (i == 0.0f)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
	}
	else if (i == 0.1f)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
	}
	else if (i == 0.2f)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
	}
	else if (i == 0.3f)
	{
		glColor3f(1.0f, 1.0f, 0.0f);
	}
	else if (i == 0.4f)
	{
		glColor3f(0.0f, 1.0f, 1.0f);
	}
	else if (i == 0.5f)
	{
		glColor3f(1.0f, 0.0f, 1.0f);
	}
	else if (i == 0.6f)
	{
		glColor3f(1.0f, 0.5f, 0.5f);
	}
	else
	{
		glColor3f(0.5f, 0.5f, 0.5f);
	}
	glVertex3f(0.0f, 1.0f - i, 0.0f);
	glVertex3f(-1.0f + i, -1.0f + i , 0.0f);
	glVertex3f(1.0f - i, -1.0f + i, 0.0f);
	glEnd();
}

//End