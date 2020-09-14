Solar System

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows with reference of Assignment:045
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
int Year = 0;
int Day = 0;
GLUquadric*quadric = NULL;

//WndProc
case WM_CHAR:
		switch (wParam)
		{
		case 'Y':
			Year = (Year + 5) % 360;
			break;
		case 'y':
			Year = (Year - 5) % 360;
			break;
		case 'D':
			Day = (Day + 10) % 360;
			break;
		case 'd':
			Day = (Day - 10) % 360;
			break;
		}
		break;
		
//display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	
	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.75f, 30, 30);
	glPopMatrix();
	glPushMatrix();
	glRotatef((GLfloat)Year, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.5f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)Day, 0.0f, 1.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.9f, 1.0f);
	gluSphere(quadric, 0.2f, 20, 20);
	glPopMatrix();

	SwapBuffers(ghdc);
}