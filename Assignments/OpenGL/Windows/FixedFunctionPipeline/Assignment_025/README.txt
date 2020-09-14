Triangle and Rectangle on same window using glTranslatef

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.5f, 0.0f, -3.0f);

	glBegin(GL_POLYGON);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.6f, 0.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, 0.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);

	glEnd();

	glLoadIdentity();
	glTranslatef(-0.5f, 0.0f, -3.0f);

	glBegin(GL_TRIANGLES);

	glColor3f(50.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.5f, 0.0f);

	glColor3f(0.0f, 50.0f, 0.0f);
	glVertex3f(-0.5f, -0.5f, 0.0f);

	glColor3f(0.0f, 0.0f, 50.0f);
	glVertex3f(0.5f, -0.5f, 0.0f);
	glEnd();

	SwapBuffers(ghdc);
}

//End