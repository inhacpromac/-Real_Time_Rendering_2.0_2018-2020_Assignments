Three Light(Red, Green and Blue) on Steady Sphere which are rotating in X, Y and Z Axis

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_015 & 49 & 57
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Source file
//Global
//Global variable declaration of toggling light
bool bLight = false;
GLfloat lightAngleZero = 0.0f;
GLfloat lightAngleOne = 0.0f;
GLfloat lightAngleTwo = 0.0f;

//Global Declaration of light configuration parameter
struct Light
{
	GLfloat Ambient[4];
	GLfloat Diffuse[4];
	GLfloat Specular[4];
	GLfloat Position[4];
}lights[3];

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat materialShininess[] = { 50.0f };

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
	//Light1
	lights[0].Ambient[0] = 0.0f;
	lights[0].Ambient[1] = 0.0f;
	lights[0].Ambient[2] = 0.0f;
	lights[0].Ambient[3] = 0.0f;

	lights[0].Diffuse[0] = 1.0f;
	lights[0].Diffuse[1] = 0.0f;
	lights[0].Diffuse[2] = 0.0f;
	lights[0].Diffuse[3] = 0.0f;

	lights[0].Specular[0] = 1.0f;
	lights[0].Specular[1] = 0.0f;
	lights[0].Specular[2] = 0.0f;
	lights[0].Specular[3] = 0.0f;

	lights[0].Position[0] = 0.0f;
	lights[0].Position[1] = 0.0f;
	lights[0].Position[2] = 0.0f;
	lights[0].Position[3] = 0.0f;

	//Light2
	lights[1].Ambient[0] = 0.0f;
	lights[1].Ambient[1] = 0.0f;
	lights[1].Ambient[2] = 0.0f;
	lights[1].Ambient[3] = 0.0f;

	lights[1].Diffuse[0] = 0.0f;
	lights[1].Diffuse[1] = 1.0f;
	lights[1].Diffuse[2] = 0.0f;
	lights[1].Diffuse[3] = 0.0f;

	lights[1].Specular[0] = 0.0f;
	lights[1].Specular[1] = 1.0f;
	lights[1].Specular[2] = 0.0f;
	lights[1].Specular[3] = 0.0f;

	lights[1].Position[0] = 0.0f;
	lights[1].Position[1] = 0.0f;
	lights[1].Position[2] = 0.0f;
	lights[1].Position[3] = 0.0f;

	//Light3
	lights[2].Ambient[0] = 0.0f;
	lights[2].Ambient[1] = 0.0f;
	lights[2].Ambient[2] = 0.0f;
	lights[2].Ambient[3] = 0.0f;

	lights[2].Diffuse[0] = 0.0f;
	lights[2].Diffuse[1] = 0.0f;
	lights[2].Diffuse[2] = 1.0f;
	lights[2].Diffuse[3] = 0.0f;

	lights[2].Specular[0] = 0.0f;
	lights[2].Specular[1] = 0.0f;
	lights[2].Specular[2] = 1.0f;
	lights[2].Specular[3] = 0.0f;

	lights[2].Position[0] = 0.0f;
	lights[2].Position[1] = 0.0f;
	lights[2].Position[2] = 0.0f;
	lights[2].Position[3] = 0.0f;
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, lights[0].Ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lights[0].Diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lights[0].Specular);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lights[1].Ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lights[1].Diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lights[1].Specular);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lights[2].Ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lights[2].Diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lights[2].Specular);
	glEnable(GL_LIGHT2);
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	
//uninitilize
if (Quadric)
	{
		gluDeleteQuadric(Quadric);
		Quadric = NULL;
	}
	
//Display

void update()
{
	if (lightAngleZero >= 360.0f)
	{
		lightAngleZero = 0.0f;
	}
	else
	{
		lightAngleZero = lightAngleZero + 0.1f;
	}

	if (lightAngleOne >= 360.0f)
	{
		lightAngleOne = 0.0f;
	}
	else
	{
		lightAngleOne = lightAngleOne + 0.1f;
	}

	if (lightAngleTwo >= 360.0f)
	{
		lightAngleTwo = 0.0f;
	}
	else
	{
		lightAngleTwo = lightAngleTwo + 0.1f;
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	gluLookAt(0.0f, 0.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	//FirstLight
	glPushMatrix();

	glRotatef(lightAngleZero, 1.0f, 0.0f, 0.0f);
	lights[0].Position[1] = lightAngleZero;
	glLightfv(GL_LIGHT0, GL_POSITION, lights[0].Position);
	
	glPopMatrix();

	//SecondLight
	glPushMatrix();

	glRotatef(lightAngleOne, 0.0f, 1.0f, 0.0f);
	lights[1].Position[2] = lightAngleOne;
	glLightfv(GL_LIGHT1, GL_POSITION, lights[1].Position);
	
	glPopMatrix();

	//ThirdLight
	glPushMatrix();

	glRotatef(lightAngleTwo, 0.0f, 0.0f, 1.0f);
	lights[2].Position[0] = lightAngleTwo;
	glLightfv(GL_LIGHT2, GL_POSITION, lights[2].Position);
	
	glPopMatrix();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Quadric = gluNewQuadric();
	gluSphere(Quadric, 0.7, 30, 30);
	glPopMatrix();

	SwapBuffers(ghdc);
}