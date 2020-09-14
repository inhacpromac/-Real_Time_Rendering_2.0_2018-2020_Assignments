Single Light on Steady Sphere with Material Property

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_015 & 44 & 49 & 55
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Source file
//Global
//Global variable declaration of toggling light
bool bLight = false;

//Global Declaration of light configuration parameter
GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 1.0f, 1.0f, 1.0f, 1.0f }; //   100.0f, 100.0f, 100.0f, 1.0f

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };
GLfloat materialShininess[] = { 128.0f };//50.0f

GLUquadric *Quadric = NULL;

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
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	
//uninitialize
if (Quadric)
	{
		gluDeleteQuadric(Quadric);
		Quadric = NULL;
	}

//Display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -0.7f);// -0.75, -0.55
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Quadric = gluNewQuadric();
	gluSphere(Quadric, 0.2f, 30, 30);
	

	SwapBuffers(ghdc);
}

