GLUT 3 - Push and Pop Matrix using freeglut

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MultiColoredTriangle:Assignment_015 Changes Made to That As Follows with reference of Assignment:046
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
int Year = 0;
int Day = 0;

//initialize
glShadeModel(GL_FLAT);

//display
void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glutWireSphere(1.0f, 20.0f, 16.0f); //Sun
	glRotatef((GLfloat)Year, 0.0f, 1.0f, 0.0f); //Revoluation
	glTranslatef(2.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)Day, 0.0f, 1.0f, 0.0f); //Rotation
	glutWireSphere(0.2f, 10.0f, 8.0f); //Earth //glBegin() and glEnd() resides under code of glutWireSphere
	glPopMatrix();

	glutSwapBuffers();
}

//Keyboard
void keyboard(unsigned char key, int x, int y)
{
	//code
	switch (key)
	{
	case 'Y':
		Year = (Year + 5) % 360;
		glutPostRedisplay();
		break;
	case 'y':
		Year = (Year - 5) % 360;
		glutPostRedisplay();
		break;
	case 'D':
		Day = (Day + 10) % 360;
		glutPostRedisplay();
		break;
	case 'd':
		Day = (Day - 10) % 360;
		glutPostRedisplay();
		break;
	case 27:
		glutLeaveMainLoop();
		break;

	case 'F':
	case 'f':
		if (bIsFullScreen == false)
		{
			glutFullScreen();
			bIsFullScreen = true;
		}
		else
		{
			glutLeaveFullScreen();
			bIsFullScreen = false;
		}
		break;
	}
}