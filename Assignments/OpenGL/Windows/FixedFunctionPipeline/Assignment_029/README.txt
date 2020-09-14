Circle using GL_Lines on Graph

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
#include<math.h>
#define PI 3.1415926535897932384626433832795
#define num_point 1000

//Changes in display()
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	GLfloat step = 1.0f / 20;

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	//X
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	//Y
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	//Top Horizontal
	for (float i = 0.0f + step; i < 1.0f; i += step)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, i, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, i, 0.0f);
	}

	//Bottom Horizontal
	for (float i = 0.0f - step; i > -1.0f; i -= step)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, i, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, i, 0.0f);
	}

	//Left Vertical
	for (float i = 0.0f - step; i > -1.0f; i -= step)
	{

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, -1.0f, 0.0f);
	}

	//Right Vertical
	for (float i = 0.0f + step; i < 1.0f; i += step)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, -1.0f, 0.0f);
	}
	glEnd();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	glBegin(GL_LINE_LOOP);

	for (int i=0.0f;i<num_point;i++)
	{
		GLfloat angle = 2.0f*PI*i / num_point;

		glColor4f(50.0f, 50.0f, 0.0f, 1.0f);
		glVertex3f(cos(angle), sin(angle), 0);
	}

	glEnd();

	SwapBuffers(ghdc);
}

//End