Robotic Arm

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_050 
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
//Remove Globally declared variables/functions for solar system
int Shoulder = 0;
int Elbow = 0;

//WndProc
//Replace
case WM_CHAR:
		switch (wParam)
		{
		case 'S':
			Shoulder = (Shoulder + 3) % 360;
			break;
		case 's':
			Shoulder = (Shoulder - 3) % 360;
			break;
		case 'E':
			Elbow = (Elbow + 3) % 360;
			break;
		case 'e':
			Elbow = (Elbow - 3) % 360;
			break;
		}
		break;

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -5.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glColor3f(0.5f, 0.35f, 0.05f);
	glTranslatef(0.0f, 0.0f, -12.0f);
	glPushMatrix();
	glRotatef((GLfloat)Shoulder, 0.0f, 0.0f, 1.0f);
	glTranslatef(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(2.0f, 0.5f, 1.0f);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.5f, 10, 10);
	glPopMatrix();
	glTranslatef(1.0f, 0.0f, 0.0f);
	glRotatef((GLfloat)Elbow, 0.0f, 0.0f, 1.0f);
	glTranslatef(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	glScalef(2.0f, 0.5f, 1.0f);
	quadric = gluNewQuadric();
	gluSphere(quadric, 0.5f, 10, 10);
	glPopMatrix();
	glPopMatrix();

	SwapBuffers(ghdc);
}

//End