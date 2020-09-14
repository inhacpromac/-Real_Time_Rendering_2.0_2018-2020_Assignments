MultiColored Triangle using Native Windowing in Windows

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied DoubleBuffer:Assignment_012 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add thses lines under display()
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_TRIANGLES);

	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(0.0f, 1.0f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(-1.0f, -1.0f);

	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(1.0f, -1.0f);
	glEnd();

	SwapBuffers(ghdc);
}


//End