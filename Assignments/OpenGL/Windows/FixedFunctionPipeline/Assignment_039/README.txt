Custom Matrix

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows with reference of Assignment_037
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
void drawTriangle();

GLfloat Identity_Matrix[16];
GLfloat Translate_Matrix[16];
GLfloat Rotate_Matrix_X[16];
GLfloat Rotate_Matrix_Y[16];
GLfloat Rotate_Matrix_Z[16];
GLfloat tx=0.0f, ty=0.0f, tz=0.0f;
GLfloat theta = 0.0f;

//Display
void display(void)
{
	if (theta > 360.0f)
	{
		theta = 0.0f;
	}

	//Identity Matrix
	Identity_Matrix[0] = 1.0f;
	Identity_Matrix[1] = 0.0f;
	Identity_Matrix[2] = 0.0f;
	Identity_Matrix[3] = 0.0f;
	Identity_Matrix[4] = 0.0f;
	Identity_Matrix[5] = 1.0f;
	Identity_Matrix[6] = 0.0f;
	Identity_Matrix[7] = 0.0f;
	Identity_Matrix[8] = 0.0f;
	Identity_Matrix[9] = 0.0f;
	Identity_Matrix[10] = 1.0f;
	Identity_Matrix[11] = 0.0f;
	Identity_Matrix[12] = 0.0f;
	Identity_Matrix[13] = 0.0f;
	Identity_Matrix[14] = 0.0f;
	Identity_Matrix[15] = 1.0f;

	//Traslate Matrix
	Translate_Matrix[0] = 1.0f;
	Translate_Matrix[1] = 0.0f;
	Translate_Matrix[2] = 0.0f;
	Translate_Matrix[3] = 0.0f;
	Translate_Matrix[4] = 0.0f;
	Translate_Matrix[5] = 1.0f;
	Translate_Matrix[6] = 0.0f;
	Translate_Matrix[7] = 0.0f;
	Translate_Matrix[8] = 0.0f;
	Translate_Matrix[9] = 0.0f;
	Translate_Matrix[10] = 1.0f;
	Translate_Matrix[11] = 0.0f;
	Translate_Matrix[12] = tx = 0.0f;
	Translate_Matrix[13] = ty = 0.0f;
	Translate_Matrix[14] = tz = -3.0f;
	Translate_Matrix[15] = 1.0f;

	//Rotate Matrix along X-axis
	Rotate_Matrix_X[0] = 1.0f;
	Rotate_Matrix_X[1] = 0.0f;
	Rotate_Matrix_X[2] = 0.0f;
	Rotate_Matrix_X[3] = 0.0f;
	Rotate_Matrix_X[4] = 0.0f;
	Rotate_Matrix_X[5] = (GLfloat)cos(theta);
	Rotate_Matrix_X[6] = (GLfloat)sin(theta);
	Rotate_Matrix_X[7] = 0.0f;
	Rotate_Matrix_X[8] = 0.0f;
	Rotate_Matrix_X[9] = (GLfloat)-sin(theta);
	Rotate_Matrix_X[10] = (GLfloat)cos(theta);
	Rotate_Matrix_X[11] = 1.0f;
	Rotate_Matrix_X[12] = 1.0f;
	Rotate_Matrix_X[13] = 1.0f;
	Rotate_Matrix_X[14] = 1.0f;
	Rotate_Matrix_X[15] = 1.0f;

	//Rotate Matrix along Y-axis
	Rotate_Matrix_Y[0] = (GLfloat)cos(theta);
	Rotate_Matrix_Y[1] = 0.0f;
	Rotate_Matrix_Y[2] = (GLfloat)-sin(theta);
	Rotate_Matrix_Y[3] = 0.0f;
	Rotate_Matrix_Y[4] = 0.0f;
	Rotate_Matrix_Y[5] = 1.0f;
	Rotate_Matrix_Y[6] = 0.0f;
	Rotate_Matrix_Y[7] = 0.0f;
	Rotate_Matrix_Y[8] = (GLfloat)sin(theta);
	Rotate_Matrix_Y[9] = 0.0f;
	Rotate_Matrix_Y[10] = (GLfloat)cos(theta);
	Rotate_Matrix_Y[11] = 0.0f;
	Rotate_Matrix_Y[12] = 0.0f;
	Rotate_Matrix_Y[13] = 0.0f;
	Rotate_Matrix_Y[14] = 0.0f;
	Rotate_Matrix_Y[15] = 1.0f;

	//Rotate Matrix along Z-axis
	Rotate_Matrix_Z[0] = (GLfloat)cos(theta);
	Rotate_Matrix_Z[1] = (GLfloat)sin(theta);
	Rotate_Matrix_Z[2] = 0.0f;
	Rotate_Matrix_Z[3] = 0.0f;
	Rotate_Matrix_Z[4] = (GLfloat)-sin(theta);
	Rotate_Matrix_Z[5] = (GLfloat)cos(theta);
	Rotate_Matrix_Z[6] = 0.0f;
	Rotate_Matrix_Z[7] = 0.0f;
	Rotate_Matrix_Z[8] = 0.0f;
	Rotate_Matrix_Z[9] = 0.0f;
	Rotate_Matrix_Z[10] = 1.0f;
	Rotate_Matrix_Z[11] = 0.0f;
	Rotate_Matrix_Z[12] = 0.0f;
	Rotate_Matrix_Z[13] = 0.0f;
	Rotate_Matrix_Z[14] = 0.0f;
	Rotate_Matrix_Z[15] = 1.0f;

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadMatrixf(Identity_Matrix);
	glMultMatrixf(Translate_Matrix);
	//glMultMatrixf(Rotate_Matrix_X);

	drawTriangle();

	SwapBuffers(ghdc);

	theta = theta + 0.1f;
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

//End