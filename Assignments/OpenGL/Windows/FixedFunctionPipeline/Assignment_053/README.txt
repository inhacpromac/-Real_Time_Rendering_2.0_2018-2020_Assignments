Tweaked Smileys

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_052 & 015
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////Resource.rc
#include "resource.h"
IDBITMAP_SMILEY BITMAP Smiley.bmp
////////////////////////////////////////////////////////////////////resource.h
#define IDBITMAP_SMILEY 101
////////////////////////////////////////////////////////////////////Source file
//Global
#include "resource.h"

GLuint Texture_Smiley;

void drawRectangle();

//WndProc
case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case 0x46:
			ToggleFullScreen();
			break;
		case 48:
		case VK_NUMPAD0:
			passedDigit = 0;
			break;
		case 49:
		case VK_NUMPAD1:
			passedDigit = 1;
			break;
		case 50:
		case VK_NUMPAD2:
			passedDigit = 2;
			break;
		case 51:
		case VK_NUMPAD3:
			passedDigit = 3;
			break;
		case 52:
		case VK_NUMPAD4:
			passedDigit = 4;
			break;
		}
		break;
		
//initialize
BOOL loadTexture(GLuint*, TCHAR[]);

glEnable(GL_TEXTURE_2D);
loadTexture(&Texture_Smiley, MAKEINTRESOURCE(IDBITMAP_SMILEY));

//uninitialize
glDeleteTextures(1, &Texture_Smiley);

//loadTexture
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
	glTranslatef(0.0f, 0.0f, -5.0f);

	drawRectangle();

	SwapBuffers(ghdc);
}

void drawRectangle()
{
	if (passedDigit == 0)         //White
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);


		glEnd();
	}
	else if (passedDigit == 1)    //BottomLeftQuater
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.0f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	else if (passedDigit == 2)    //FullTexture
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	else if (passedDigit == 4)    //Centered
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	else if (passedDigit = 3)    //Tilled
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(2.0f, 2.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.0f, 2.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f(2.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	
}
