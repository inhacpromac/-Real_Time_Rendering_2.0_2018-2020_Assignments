Static INDIA

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
void drawI();
void drawN();
void drawD();
void drawA();

//Display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-3.5f, 0.0f, -10.0f);
	
	drawI();

	glLoadIdentity();
	glTranslatef(-2.75f, 0.0f, -10.0f);

	drawN();

	glLoadIdentity();
	glTranslatef(-0.6f, 0.0f, -10.0f);

	drawD();

	glLoadIdentity();
	glTranslatef(1.15f, 0.0f, -10.0f);

	drawI();

	glLoadIdentity();
	glTranslatef(1.9f, 0.0f, -10.0f);

	drawA();

	SwapBuffers(ghdc);
}

void drawI()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.2f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, -0.2f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, -0.2f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.2f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);
	glEnd();
}

void drawN()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 0.6f);	

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.8f, -1.5f);

	/*glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -0.6f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.8f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.8f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.8f, 1.5f);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.4f, 0.0f);*/

	glEnd();	
}

void drawD()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.0f, 1.5f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.4f, 1.1f);
	glVertex2f(1.0f, 1.1f);
	
	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);
	glVertex2f(0.4f, -1.1f);
	glVertex2f(1.0f, -1.1f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(1.4f, -1.1f);

	glEnd();
}

void drawA()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.1f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.1f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(1.4f, -1.5f);

	glEnd();
	//OrangeFlag
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.0f, 0.3f);
	glVertex2f(0.4f, 0.3f);
	glVertex2f(0.4f, 0.1f);
	glVertex2f(1.0f, 0.1f);

	glEnd();
	//WhiteFlag
	glBegin(GL_POLYGON);
	
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.0f, 0.1f);
	glVertex2f(0.4f, 0.1f);
	glVertex2f(0.4f, -0.1f);
	glVertex2f(1.0f, -0.1f);

	glEnd();
	//GreenFlag
	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -0.1f);
	glVertex2f(0.4f, -0.1f);
	glVertex2f(0.4f, -0.3f);
	glVertex2f(1.0f, -0.3f);

	glEnd();
	
}