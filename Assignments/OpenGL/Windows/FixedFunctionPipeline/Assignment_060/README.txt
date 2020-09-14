Model Loading Using Header File with Texture and Light

///////////////////////////////////////////////////////////////////////////////////////////////////
//Refer:Assignment_015 & 53 & 56
///////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////resource.h
#define IDBITMAP_MARBLE 101
////////////////////////////////////////////////////////////////////Resource.rc
#include "resource.h"
IDBITMAP_MARBLE BITMAP marble.bmp
////////////////////////////////////////////////////////////////////Source file
//Global
#include "OGL.h"
#include "resource.h"

GLfloat angleTeapot = 0.0f;
GLuint Texture_Marble;
bool bTexture = false;
//Global variable declaration of toggling light
bool bLight = false;

//Global Declaration of light configuration parameter
GLfloat lightAmbient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 100.0f, 100.0f, 100.0f, 1.0f };

//WndProc
case WM_CHAR:
		switch (wParam)
		{
		case 'L':
		case 'l':
			if (bLight == false)
			{
				bLight = true;
				glEnable(GL_LIGHTING);
			}
			else
			{
				bLight = false;
				glDisable(GL_LIGHTING);
			}
			break;
		case 'T':
		case 't':
			if (bTexture == false)
			{
				bTexture = true;
				glEnable(GL_TEXTURE_2D);
			}
			else
			{
				bTexture = false;
				glDisable(GL_TEXTURE_2D);
			}
		}
		break;
		
//initialize
BOOL loadTexture(GLuint*, TCHAR[]);

glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT0);
	loadTexture(&Texture_Marble, MAKEINTRESOURCE(IDBITMAP_MARBLE));

//display
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2.0f);
	//glRotatef(-90, 0.0f, 1.0f, 0.0f);
	glRotatef(angleTeapot, 0.0f, 1.0f, 0.0f);
	glBindTexture(GL_TEXTURE_2D, Texture_Marble);

	glBegin(GL_TRIANGLES);

	for (int i = 0; i < sizeof(face_indicies) / sizeof(face_indicies[0]); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			int vi = face_indicies[i][j];
			int ni = face_indicies[i][j+3];
			int ti = face_indicies[i][j+6];

			glTexCoord2f(textures[ti][0], textures[ti][1]);
			glNormal3f(normals[ni][0], normals[ni][1], normals[ni][2]);
			glVertex3f(vertices[vi][0], vertices[vi][1], vertices[vi][2]);
		}
	}

	glEnd();

	SwapBuffers(ghdc);
}

void update(void)
{
	if (angleTeapot > 360.f)
	{
		angleTeapot = 0.0f;
	}

	angleTeapot = angleTeapot + 0.1f;
}