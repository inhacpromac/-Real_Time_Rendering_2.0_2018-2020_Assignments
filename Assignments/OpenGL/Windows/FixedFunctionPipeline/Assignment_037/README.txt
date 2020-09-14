2D Animation of Spinning Triangle And Pitching Rectangle

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows with reference of Assignment_025
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
void drawTriangle();
void drawRectangle();
void drawGraph();

//Display
GLfloat spinning_angle = 0.0f;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	drawGraph();

	glLoadIdentity();
	glTranslatef(-0.5f, 0.0f, -3.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawTriangle();

	glLoadIdentity();
	glTranslatef(0.5f, 0.0f, -3.0f);
	glRotatef(spinning_angle, 1.0f, 0.0f, 0.0f);

	drawRectangle();

	SwapBuffers(ghdc);

	spinning_angle = spinning_angle + 0.1f;
}

void drawGraph()
{
	GLfloat step = 1.0f / 20;

	glLineWidth(2.0f);
	glBegin(GL_LINES);
	//X
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	//Y
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();

	glLineWidth(1.0f);
	glBegin(GL_LINES);
	//Top Horizontal
	for (float i = 0.0f + step; i < 1.0f; i += step)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, i, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, i, 0.0f);
	}

	//Bottom Horizontal
	for (float i = 0.0f - step; i > -1.0f; i -= step)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, i, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, i, 0.0f);
	}

	//Left Vertical
	for (float i = 0.0f - step; i > -1.0f; i -= step)
	{

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, -1.0f, 0.0f);
	}

	//Right Vertical
	for (float i = 0.0f + step; i < 1.0f; i += step)
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, 1.0f, 0.0f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(i, -1.0f, 0.0f);
	}
	glEnd();
}

void drawTriangle()
{
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, -0.5f);

	glEnd();
}

void drawRectangle()
{
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.6f, 0.5f, 0.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);

	glEnd();

}

//End