'X' axis (Horizontal Line) using GL_LINES 

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Changes  in display()
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glLineWidth(3.0f);

	glBegin(GL_LINES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-1.0f, 0.0f, 0.0f);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);

	glEnd();

	SwapBuffers(ghdc);
}

//End