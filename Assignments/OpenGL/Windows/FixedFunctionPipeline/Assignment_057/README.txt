Two Lights(Red and Blue) on Pyramid Rotating

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_015 & 43 & 56
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Source file
//Global
GLfloat spinning_angle = 0.0f;

//Global variable declaration of toggling light
bool bLight = false;

//Global Declaration of light configuration parameter
struct Light
{
	GLfloat Ambient[4];
	GLfloat Diffuse[4];
	GLfloat Specular[4];
	GLfloat Position[4];
}lights[2];

void drawPyramid();
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
	//Light1
	lights[0].Ambient[0] = 0.0f;
	lights[0].Ambient[1] = 0.0f;
	lights[0].Ambient[2] = 0.0f;
	lights[0].Ambient[3] = 1.0f;

	lights[0].Diffuse[0] = 1.0f;
	lights[0].Diffuse[1] = 0.0f;
	lights[0].Diffuse[2] = 0.0f;
	lights[0].Diffuse[3] = 1.0f;

	lights[0].Specular[0] = 1.0f;
	lights[0].Specular[1] = 0.0f;
	lights[0].Specular[2] = 0.0f;
	lights[0].Specular[3] = 1.0f;

	lights[0].Position[0] = -2.0f;
	lights[0].Position[1] = 0.0f;
	lights[0].Position[2] = 0.0f;
	lights[0].Position[3] = 1.0f;

	//Light2
	lights[1].Ambient[0] = 0.0f;
	lights[1].Ambient[1] = 0.0f;
	lights[1].Ambient[2] = 0.0f;
	lights[1].Ambient[3] = 1.0f;

	lights[1].Diffuse[0] = 0.0f;
	lights[1].Diffuse[1] = 0.0f;
	lights[1].Diffuse[2] = 1.0f;
	lights[1].Diffuse[3] = 1.0f;

	lights[1].Specular[0] = 0.0f;
	lights[1].Specular[1] = 0.0f;
	lights[1].Specular[2] = 1.0f;
	lights[1].Specular[3] = 1.0f;

	lights[1].Position[0] = 2.0f;
	lights[1].Position[1] = 0.0f;
	lights[1].Position[2] = 0.0f;
	lights[1].Position[3] = 1.0f;
	
glShadeModel(GL_SMOOTH);
glLightfv(GL_LIGHT0, GL_AMBIENT, lights[0].Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[0].Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lights[0].Specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lights[0].Position);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lights[1].Ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lights[1].Diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lights[1].Specular);
	glLightfv(GL_LIGHT1, GL_POSITION, lights[1].Position);
	glEnable(GL_LIGHT1);
	
//display
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
	glNormal3f(0.0f, 0.447214f, 0.894427f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-corner of front face
	glVertex3f(1.0f, -1.0f, 1.0f); //right-corner of front face

	//RIGHT FACE
	glNormal3f(0.894427f, 0.447214f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glVertex3f(1.0f, -1.0f, 1.0f); //left-corner of right face
	glVertex3f(1.0f, -1.0f, -1.0f); //right-corner of right face

	//BACK FACE
	glNormal3f(0.0f, 0.447214f, -0.894427f);
	glVertex3f(0.0, 1.0f, 0.0f); //apex
	glVertex3f(1.0f, -1.0f, -1.0f); //left-corner of back face
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-corner of back face

	//LEFT FACE
	glNormal3f(-0.894427f, 0.447214f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-corner of left face
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
