Cube with light

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_015 & 44
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Source file
//Global

//Global variable declaration of toggling light
bool bLight = false;
//Global Declaration of light configuration parameter
GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };

void drawCube();
void spin(void);

//WndProc
case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			if (bLight == false)
			{
				bLight = true;
				glEnable(GL_LIGHTING);
			}
			else
			{
				bLight = false;
				glDisable(GL_LIGHTING);
			}
			break;
		}
		break;
		
//initialize
glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
glEnable(GL_LIGHT0);
	
//display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -6.0f);
	glRotatef(spinning_angle, 1.0f, 1.0f, 1.0f);

	drawCube();

	SwapBuffers(ghdc);
}

void drawCube()
{
	glBegin(GL_QUADS);

	//TOP FACE
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top corner of top face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top corner of top face
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-bottom corner of top face
	glVertex3f(1.0f, 1.0f, 1.0f); //right-bottom of top face

	//BOTTOM FACE
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //right-top corner of bottom face
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-top corner of bottom face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom corner of bottom face
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom corner of bottom face

	//FRONT FACE
	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); //right-top corner of front face
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-top corner of front face
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-bottom corner of front face
	glVertex3f(1.0f, -1.0f, 1.0f); //right-bottom corner of front fcae

	//BACK FACE
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //right-top of back face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-top of back face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-bottom of back face
	glVertex3f(1.0f, 1.0f, -1.0f); //right-bottom of back face

	//LEFT FACE
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); //right-top of left face
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top of left face
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom of left face
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-bottom of left face

	//RIGHT FACE
	glNormal3f(1.0f, 0.0f, 0.0f);
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