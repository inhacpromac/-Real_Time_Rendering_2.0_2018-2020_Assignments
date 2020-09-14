Spinning DeathlyHallows

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows with reference of Assignment_036 & 037
///////////////////////////////////////////////////////////////////////////////////////////////////

//Global
void drawTriangle();
void drawCircle();
void drawLine();

//Display
GLfloat X1 = 0.0f, Y1 = 0.5f;
GLfloat X2 = -0.5f, Y2 = -0.5f;
GLfloat X3 = 0.5f, Y3 = -0.5f;
GLfloat angle = 0.0f;
GLfloat spinning_angle = 0.0f;
GLfloat radius = 0.0f;
GLfloat s = 0.0f;
GLfloat a = 0.0f;
GLfloat b = 0.0f;
GLfloat c = 0.0f;
GLfloat Xc = 0.0f, Yc = 0.0f;

void display(void)
{
	//Distance of Sides of Triangle
	a = (GLfloat)sqrt((X2 - X1)*(X2 - X1) + (Y2 - Y1)*(Y2 - Y1));
	b = (GLfloat)sqrt((X3 - X2)*(X3 - X2) + (Y3 - Y2)*(Y3 - Y2));
	c = (GLfloat)sqrt((X1 - X3)*(X1 - X3) + (Y1 - Y3)*(Y1 - Y3));

	s = (a + b + c) / 2;

	//Radius of Incircle
	radius = (GLfloat)sqrt(s*(s - a)*(s - b)*(s - c)) / s;

	//Centers of Incircle
	Xc = ((b*X1) + (c*X2) + (a*X3)) / (a + b + c);
	Yc = ((b*Y1) + (c*Y2) + (a*Y3)) / (a + b + c);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawTriangle();

	glLoadIdentity();
	glTranslatef(Xc, Yc, -3.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawCircle();

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawLine();

	SwapBuffers(ghdc);

	spinning_angle = spinning_angle + 0.1f;
}

void drawTriangle()
{
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(X1, Y1);
	glVertex2f(X2, Y2);
	glVertex2f(X3, Y3);

	glEnd();
}

void drawCircle()
{
	glBegin(GL_LINE_LOOP);

	for (angle = 0.0f; angle < 2 * M_PI; angle += 0.05f)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f((GLfloat)(cos(angle)*radius), (GLfloat)(sin(angle)*radius));
	}

	glEnd();
}

void drawLine()
{
	glBegin(GL_LINES);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.0f, -0.5f);

	glEnd();
}

//End