3DTexture - Textures on Rotating Pyramid and Cube

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_045
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Resource.rc
#include "resource.h"
IDBITMAP_STONE BITMAP Stone.bmp
IDBITMAP_KUNDALI BITMAP Vijay_Kundali.bmp
////////////////////////////////////////////////////////////////////resource.h
#define IDBITMAP_STONE 101
#define IDBITMAP_KUNDALI 102
////////////////////////////////////////////////////////////////////Source file
//Global
#include "resource.h"

GLfloat spinning_angle = 0.0f;
GLuint Texture_Stone;
GLuint Texture_Kundali;

void drawPyramid();
void drawCube();
void spin(void);

//initialize
BOOL loadTexture(GLuint*, TCHAR[]);

glEnable(GL_TEXTURE_2D);
loadTexture(&Texture_Stone, MAKEINTRESOURCE(IDBITMAP_STONE));
loadTexture(&Texture_Kundali, MAKEINTRESOURCE(IDBITMAP_KUNDALI));

//unintialize
glDeleteTextures(1, &Texture_Stone);
glDeleteTextures(1, &Texture_Kundali);

//Load Texture
BOOL loadTexture(GLuint *Texture, TCHAR imageResourceID[])
{
	// variable declaration
	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	BOOL bStatus = FALSE;

	// code
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceID, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
	if (hBitmap)
	{
		bStatus = TRUE;
		GetObject(hBitmap, sizeof(BITMAP), &bmp);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glGenTextures(1, Texture);
		glBindTexture(GL_TEXTURE_2D, *Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		DeleteObject(hBitmap);
	}
	return(bStatus);
}

//Display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(-1.5f, 0.0f, -7.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, Texture_Stone);

	drawPyramid();

	glLoadIdentity();
	glTranslatef(1.5f, 0.0f, -7.0f);
	glScalef(0.75, 0.75, 0.75);
	glRotatef(spinning_angle, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, Texture_Kundali);

	drawCube();

	SwapBuffers(ghdc);
}

void drawPyramid()
{
	glBegin(GL_TRIANGLES);

	//FRONT FACE
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-corner of front face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //right-corner of front face

	//RIGHT FACE
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //left-corner of right face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //right-corner of right face

	//BACK FACE
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0, 1.0f, 0.0f); //apex
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //left-corner of back face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //right-corner of back face

	//LEFT FACE
	glTexCoord2f(0.5f, 1.0f);
	glVertex3f(0.0f, 1.0f, 0.0f); //apex
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-corner of left face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f);  //right-corner of left face

	glEnd();
}

void drawCube()
{
	glBegin(GL_QUADS);

	//FRONT FACE
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); //right-top corner of front face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-top corner of front face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-bottom corner of front face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //right-bottom corner of front fcae

	//RIGHT FACE
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top of right face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); //left-top of right face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //left-bottom of right face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom of right face

	//BACK FACE
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //right-top of back face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-top of back face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-bottom of back face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); //right-bottom of back face

	//LEFT FACE
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); //right-top of left face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top of left face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom of left face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //right-bottom of left face

	//TOP FACE
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, -1.0f); //right-top corner of top face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, -1.0f); //left-top corner of top face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-1.0f, 1.0f, 1.0f); //left-bottom corner of top face
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, 1.0f, 1.0f); //right-bottom of top face

	//BOTTOM FACE
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(1.0f, -1.0f, 1.0f); //right-top corner of bottom face
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1.0f, -1.0f, 1.0f); //left-top corner of bottom face
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, -1.0f); //left-bottom corner of bottom face
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, -1.0f); //right-bottom corner of bottom face

	glEnd();
}

void spin(void)
{
	if (spinning_angle > 360.f)
	{
		spinning_angle = 0.0f;
	}

	spinning_angle = spinning_angle + 0.1f;
}
////////////////////////////////////////////////////////////////////

//End