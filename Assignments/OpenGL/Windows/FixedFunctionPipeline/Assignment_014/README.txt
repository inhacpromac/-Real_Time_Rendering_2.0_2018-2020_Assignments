Ortho

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied DoubleBuffer:Assignment_012 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add thses lines under resize()
void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	if (width <= height)
	{
		glOrtho(-100.0f, 100.0f, -100.0f*GLfloat(height) / GLfloat(width), 100.0f*GLfloat(height) / GLfloat(width), -100.0f, 100.0f);
	}
	else
	{
		glOrtho(-100.0f*GLfloat(width) / GLfloat(height), 100.0f*GLfloat(width) / GLfloat(height), -100.0f, 100.0f, -100.0f, 100.0f);
	}
}

//Add thses lines under display() same from Assignment_013:Triangle
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glBegin(GL_TRIANGLES);

	glColor3f(50.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 50.0f);

	glColor3f(0.0f, 50.0f, 0.0f);
	glVertex2f(-50.0f, -50.0f);

	glColor3f(0.0f, 0.0f, 50.0f);
	glVertex2f(50.0f, -50.0f);
	glEnd();

	SwapBuffers(ghdc);
}

//End