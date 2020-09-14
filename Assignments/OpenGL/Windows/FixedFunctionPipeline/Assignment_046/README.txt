GLUT 2 - Pyramid and Cube Animation using freeglut

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MultiColoredTriangle:Assignment_015 Changes Made to That As Follows with reference of Assignment:045
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
GLfloat angle_pyramid = 0.0f;
GLfloat angle_cube = 0.0f;

void drawPyramid(void);
void drawCube(void);
void update(void);

//main
glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
//main-callabacks
glutIdleFunc(update);

//initialize
glShadeModel(GL_SMOOTH);
glClearDepth(1.0f);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LEQUAL);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

//reshape
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
	gluPerspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

//display
void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -7.0f);
	glRotatef(angle_pyramid, 0.0f, 1.0f, 0.0f);

	drawPyramid();

	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -7.0f);
	glScalef(0.75, 0.75, 0.75);
	glRotatef(angle_cube, 1.0f, 1.0f, 1.0f);

	drawCube();

	glutSwapBuffers();
}

void drawPyramid()
{
	glBegin(GL_TRIANGLES);

	//FRONT FACE
	glColor3f(1.0f, 0.0f, 0.0f); // RED
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 1.0f, 0.0f); // GREEN
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-corner of front face
	glColor3f(0.0f, 0.0f, 1.0f); // BLUE
	glVertex3f(1.0f, -1.0f, 1.0f); //right-corner of front face

	//RIGHT FACE
	glColor3f(1.0f, 0.0f, 0.0f); // RED
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 0.0f, 1.0f); // BLUE
	glVertex3f(1.0f, -1.0f, 1.0f); //left-corner of right face
	glColor3f(0.0f, 1.0f, 0.0f); // GREEN
	glVertex3f(1.0f, -1.0f, -1.0f); //right-corner of right face

	//BACK FACE
	glColor3f(1.0f, 0.0f, 0.0f); // RED
	glVertex3f(0.0, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 1.0f, 0.0f); // GREEN
	glVertex3f(1.0f, -1.0f, -1.0f); //left-corner of back face
	glColor3f(0.0f, 0.0f, 1.0f); // BLUE
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-corner of back face

	//LEFT FACE
	glColor3f(1.0f, 0.0f, 0.0f); // RED
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glColor3f(0.0f, 0.0f, 1.0f); // BLUE
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-corner of left face
	glColor3f(0.0f, 1.0f, 0.0f); // GREEN
	glVertex3f(-1.0f, -1.0f, 1.0f);  //right-corner of left face

	glEnd();
}

void drawCube()
{
	glBegin(GL_QUADS);

	//TOP FACE
	glColor3f(1.0f, 0.0f, 0.0f); // RED
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top corner of top face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top corner of top face
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-bottom corner of top face
	glVertex3f(1.0f, 1.0f, 1.0f); //right-bottom of top face

	//BOTTOM FACE
	glColor3f(0.0f, 1.0f, 0.0f); // GREEN
	glVertex3f(1.0f, -1.0f, 1.0f); //right-top corner of bottom face
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-top corner of bottom face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom corner of bottom face
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom corner of bottom face

	//FRONT FACE
	glColor3f(0.0f, 0.0f, 1.0f); // BLUE
	glVertex3f(1.0f, 1.0f, 1.0f); //right-top corner of front face
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-top corner of front face
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-bottom corner of front face
	glVertex3f(1.0f, -1.0f, 1.0f); //right-bottom corner of front fcae

	//BACK FACE
	glColor3f(0.0f, 1.0f, 1.0f); //CYAN
	glVertex3f(1.0f, -1.0f, -1.0f); //right-top of back face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-top of back face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-bottom of back face
	glVertex3f(1.0f, 1.0f, -1.0f); //right-bottom of back face

	//LEFT FACE
	glColor3f(1.0f, 0.0f, 1.0f); //MAGENTA
	glVertex3f(-1.0f, 1.0f, 1.0f); //right-top of left face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top of left face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom of left face
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-bottom of left face

	//RIGHT FACE
	glColor3f(1.0f, 1.0f, 0.0f); //YELLOW
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top of right face
	glVertex3f(1.0f, 1.0f, 1.0f); //left-top of right face
	glVertex3f(1.0f, -1.0f, 1.0f); //left-bottom of right face
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom of right face

	glEnd();
}

void update(void)
{
	if (angle_pyramid > 360.f)
	{
		angle_pyramid = 0.0f;
	}
	if (angle_cube > 360.f)
	{
		angle_cube = 0.0f;
	}

	angle_pyramid = angle_pyramid + 0.1f;
	angle_cube = angle_cube + 0.1f;

	glutPostRedisplay();
}