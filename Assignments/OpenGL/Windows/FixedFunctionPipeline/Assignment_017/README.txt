Point using GL_POINTS

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//changes in display()
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	SwapBuffers(ghdc);
}

//End