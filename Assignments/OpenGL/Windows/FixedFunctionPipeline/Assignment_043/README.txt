Pyramid 3D

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
#define _USE_MATH_DEFINES 1
#include <math.h>

GLfloat spinning_angle = 0.0f;

void uninitialize();
void drawPyramid();
void spin(void);

/////////////////////////////
call spin(); in gameloop
/////////////////////////////

//initialize
glClearDepth(1.0f);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LEQUAL);
glShadeModel(GL_SMOOTH);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

//Display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawPyramid();

	SwapBuffers(ghdc);
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