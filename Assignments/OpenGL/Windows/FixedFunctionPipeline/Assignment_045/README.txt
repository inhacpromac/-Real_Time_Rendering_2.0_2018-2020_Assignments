3D Animation

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows with reference of Assignment:043 &44
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
GLfloat spinning_angle = 0.0f;

void drawPyramid();
void drawCube();
void spin(void);

//Display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -7.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawPyramid();

	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -7.0f);
	glScalef(0.75, 0.75, 0.75);
	glRotatef(spinning_angle, 1.0f, 1.0f, 1.0f);

	drawCube();

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

void spin(void)
{
	if (spinning_angle > 360.f)
	{
		spinning_angle = 0.0f;
	}

	spinning_angle = spinning_angle + 0.1f;
}