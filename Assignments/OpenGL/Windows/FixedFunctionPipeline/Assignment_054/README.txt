Checkerboard

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_015
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Source file
//Global

#define checkImageWidth 64
#define checkImageHeight 64

GLubyte checkImage[checkImageHeight][checkImageWidth][4];
GLuint texImage;

//initialize
glEnable(GL_TEXTURE_2D);
loadTexture();

//uninitialize
glDeleteTextures(1, &texImage);

//loadTexture
void loadTexture(void)
{
	void makeCheckImage(void);

	//code
	makeCheckImage();
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texImage);
	glBindTexture(GL_TEXTURE_2D, texImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void makeCheckImage(void)
{
	int i, j, c;

	for (i = 0; i < checkImageHeight; i++)
	{
		for (j = 0; j < checkImageWidth; j++)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}

//display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.6f);

	glBegin(GL_QUADS);

	glTexCoord3f(0.0f, 0.0f, 0.0f);
	glVertex3f(-2.0f, -1.0f, 0.0f);

	glTexCoord3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-2.0f, 1.0f, 0.0f);

	glTexCoord3f(1.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.0f, 0.0f);

	glTexCoord3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);

	glEnd();

	glBegin(GL_QUADS);

	glTexCoord3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);

	glTexCoord3f(0.0f, 1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 0.0f);

	glTexCoord3f(1.0f, 1.0f, 0.0f);
	glVertex3f(2.41421f, 1.0f, -1.41421f);

	glTexCoord3f(1.0f, 0.0f, 0.0f);
	glVertex3f(2.41421f, -1.0f, -1.41421f);

	glEnd();

	//glFlush();
	SwapBuffers(ghdc);
}



//End