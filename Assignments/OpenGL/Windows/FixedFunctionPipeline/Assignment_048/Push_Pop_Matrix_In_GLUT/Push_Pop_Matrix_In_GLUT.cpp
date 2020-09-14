// Assignment_048
// Push_Pop_Matrix_In_GLUT

// Header
#include<GL/freeglut.h>

bool bIsFullScreen = false;
int Year = 0;
int Day = 0;

int main(int argc, char *argv[])
{
	// function declaration
	void initialize(void);
	void uninitialize(void);
	void reshape(int, int);
	void display(void);
	void keyboard(unsigned char, int, int);
	void mouse(int, int, int, int);

	//code
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("My First OpenGL Program - Rohan V. Waghmode");

	initialize();

	// callbacks
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutCloseFunc(uninitialize);

	glutMainLoop();

	return (0);
}

void initialize(void)
{
	// code
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glShadeModel(GL_FLAT);
}

void uninitialize(void)
{
	// code
}

void reshape(int width, int height)
{
	// code
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, GLfloat(width) / GLfloat(height), 1.0f, 20.0f);
}

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

void mouse(int button, int state, int x, int y)
{
	//code
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		break;

	case GLUT_RIGHT_BUTTON:
		glutLeaveFullScreen;
		break;
	}
}
