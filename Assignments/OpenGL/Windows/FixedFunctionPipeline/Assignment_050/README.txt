Solar System V1 (with moon code)

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_050
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
int Moon_Year = 0;
int Moon_Day = 0;

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
		case 'M':
			Moon_Year = (Moon_Year + 15) % 360;
			break;
		case 'm':
			Moon_Year = (Moon_Year - 15) % 360;
			break;
		case 'N':
			Moon_Day = (Moon_Day + 20) % 360;
			break;
		case 'n':
			Moon_Day = (Moon_Day - 20) % 360;
			break;
		}
		break;
		
//Display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);

	gluLookAt(0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	// Sun
	glPushMatrix();
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	quadric = gluNewQuadric();
	glColor3f(1.0f, 1.0f, 0.0f);
	gluSphere(quadric, 0.75f, 30, 30);
	glPopMatrix();
	// Earth
	glPushMatrix();
	glRotatef((GLfloat)Year, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.5f, 0.0f, 0.0f);
	//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)Day, 0.0f, 1.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.9f, 1.0f);
	gluSphere(quadric, 0.2f, 20, 20);
	// Moon
	glRotatef((GLfloat)Moon_Year, 0.0f, 1.0f, 0.0f);
	glTranslatef(0.5f, 0.0f, 0.0f);
	glRotatef((GLfloat)Moon_Day, 0.0f, 1.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	quadric = gluNewQuadric();
	glColor3f(0.9608f, 0.9529f, 0.8078f);
	gluSphere(quadric, 0.1f, 10, 10);
	glPopMatrix();
	/*glPushMatrix();
	glRotatef((GLfloat)Moon_Year, 0.0f, 1.0f, 0.0f);
	glTranslatef(1.5f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)Moon_Day, 0.0f, 1.0f, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
	quadric = gluNewQuadric();
	glColor3f(0.4f, 0.9f, 1.0f);
	gluSphere(quadric, 0.2f, 20, 20);
	glPopMatrix();*/

	SwapBuffers(ghdc);
}