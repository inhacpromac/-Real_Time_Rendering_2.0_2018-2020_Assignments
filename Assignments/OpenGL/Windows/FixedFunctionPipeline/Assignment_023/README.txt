Rectangle using GL_LINE_LOOP

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Changes in display()
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex3f(0.5f, 0.4f, 0.0f);
	glVertex3f(-0.5f, 0.4f, 0.0f);
	glVertex3f(-0.5f, -0.4f, 0.0f);
	glVertex3f(0.5f, -0.4f, 0.0f);

	glEnd();

	SwapBuffers(ghdc);
}

//End