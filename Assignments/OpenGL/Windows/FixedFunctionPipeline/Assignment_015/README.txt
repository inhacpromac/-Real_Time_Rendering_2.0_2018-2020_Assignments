Perpective

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Ortho:Assignment_014 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add these lines under
//headers
#include<gl/GLU.h>

#pragma comment(lib,"glu32.lib")

//Replace these lines under resize()
void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

void drawPyramid()
{
	glBegin(GL_TRIANGLES);

	//FRONT FACE
	glColor3f(1.0f, 0.0f, 0.0f); // red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 1.0f, 0.0f); // green
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-corner of front face
	glColor3f(0.0f, 0.0f, 1.0f); // blue
	glVertex3f(1.0f, -1.0f, 1.0f); //right-corner of front face

	//RIGHT FACE
	glColor3f(1.0f, 0.0f, 0.0f); // red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 0.0f, 1.0f); // blue
	glVertex3f(1.0f, -1.0f, 1.0f); //left-corner of right face
	glColor3f(0.0f, 1.0f, 0.0f); // green
	glVertex3f(1.0f, -1.0f, -1.0f); //right-corner of right face

	//BACK FACE
	glColor3f(1.0f, 0.0f, 0.0f); // red
	glVertex3f(0.0, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 1.0f, 0.0f); // green
	glVertex3f(1.0f, -1.0f, -1.0f); //left-corner of back face
	glColor3f(0.0f, 0.0f, 1.0f); // blue
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-corner of back face

	//LEFT FACE
	glColor3f(1.0f, 0.0f, 0.0f); // red
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 0.0f, 1.0f); // blue
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-corner of left face
	glColor3f(0.0f, 1.0f, 0.0f); // green
	glVertex3f(-1.0f, -1.0f, 1.0f);  //right-corner of left face
	
	glEnd();
}

void spin(void)
{
	if (spinning_angle > 360.f)
	{
		spinning_angle = 0.0f;
	}

	spinning_angle = spinning_angle + 0.1f;
}

//End