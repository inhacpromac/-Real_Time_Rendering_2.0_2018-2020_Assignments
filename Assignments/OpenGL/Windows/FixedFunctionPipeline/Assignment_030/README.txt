Kundali

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2.0f);

	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.3f);
	glVertex2f(-0.4f, 0.3f);
	glVertex2f(-0.4f, -0.3f);
	glVertex2f(0.4f, -0.3f);

	glEnd();
	
	glBegin(GL_LINE_LOOP);

	glVertex2f(0.4f, 0.0f);
	glVertex2f(0.0f, 0.3f);
	glVertex2f(-0.4f, 0.0f);
	glVertex2f(0.0f, -0.3f);

	glEnd();

	glBegin(GL_LINES);

	glVertex2f(-0.4f,-0.3f);
	glVertex2f(0.4f,0.3f);
	glVertex2f(-0.4f,0.3f);
	glVertex2f(0.4f,-0.3f);

	glEnd();
	
	SwapBuffers(ghdc);
}

//End