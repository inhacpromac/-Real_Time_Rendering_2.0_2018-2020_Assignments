// Assignment_059
// 24_Spheres

// header
#include<windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// callback
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global function declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL; // OpenGL Rendering Context
FILE *gpfile = NULL;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool bDone = false;
bool gbActiveWindow = false;
bool gbFullScreen = false;
GLfloat spinning_angle = 0.0f;

//Global variable declaration of toggling light
bool bLight = false;
GLfloat angleOfXRotation = 0.0f;
GLfloat angleOfYRotation = 0.0f;
GLfloat angleOfZRotation = 0.0f;
GLint keyPressed = 0;

//Global Declaration of light configuration parameter
GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

GLUquadric *Quadric[24];

void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();
void update();
void draw24Spheres();

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// fuction declarations
	int initialize(void);
	void display(void);

	// variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd = NULL;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	int iRet = 0;

	// code
	if (fopen_s(&gpfile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Cannot be Created"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpfile, "Log File Successfully Created\n");
		fprintf(gpfile, "By Rohan\n");
	}

	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register above class
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName,
		TEXT("My Window - Rohan V. Waghmode"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	iRet = initialize();

	if (iRet == -1)
	{
		fprintf(gpfile, "Choose Pixel Format Failed\n");
	}
	else if (iRet == -2)
	{
		fprintf(gpfile, "Set Pixel Format Failed\n");
	}
	else if (iRet == -3)
	{
		fprintf(gpfile, "wglCreateContext failed\n");
	}
	else if (iRet == -4)
	{
		fprintf(gpfile, "wglMakeCurrent failed\n");
		DestroyWindow(hwnd);
	}
	else
	{
		fprintf(gpfile, "Initialization Successful\n");
	}

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Gameloop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				// Here call update
				update();
			}
			display();
		}


	}
	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActiveWindow = true;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = false;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
	case WM_ERASEBKGND:
		return (0);
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case 0x46:
			ToggleFullScreen();
			break;
		}
		break;
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
		case 'X':
		case 'x':
			keyPressed = 1;
			angleOfXRotation = 0.0f;
			break;
		case 'Y':
		case 'y':
			keyPressed = 2;
			angleOfYRotation = 0.0f;
			break;
		case 'Z':
		case 'z':
			keyPressed = 3;
			angleOfZRotation = 0.0f;
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

int initialize(void)
{
	// function declaration
	void resize(int, int);

	// variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	// initialize PDF structure / Filling Form
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	ghdc = GetDC(ghwnd);
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

	if (iPixelFormatIndex == 0)
	{
		return (-1);
	}
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == false)
	{
		return (-2);
	}

	ghrc = wglCreateContext(ghdc);

	if (ghrc == NULL)
	{
		return (-3);
	}
	if (wglMakeCurrent(ghdc, ghrc) == false)
	{
		return (-4);
	}

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);
	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++)
	{
		Quadric[i] = gluNewQuadric();
	}

	resize(WIN_WIDTH, WIN_HEIGHT);

	return (0);
}

void resize(int width, int height)
{
	if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width <= height)
	{
		glOrtho(0.0f, 15.5f, 0.0f, 15.5f*GLfloat(height) / GLfloat(width), -10.0f, 10.0f);
	}
	else
	{
		glOrtho(0.0f, 15.5f*GLfloat(width) / GLfloat(height), 0.0f, 15.5f, -10.0f, 10.0f);
	}
}

void update()
{
	if (angleOfXRotation >= 360.0f)
	{
		angleOfXRotation = 0.0f;
	}
	else
	{
		angleOfXRotation = angleOfXRotation + 0.1f;
	}
	if (angleOfYRotation >= 360.0f)
	{
		angleOfYRotation = 0.0f;
	}
	else
	{
		angleOfYRotation = angleOfYRotation + 0.1f;
	}
	if (angleOfZRotation >= 360.0f)
	{
		angleOfZRotation = 0.0f;
	}
	else
	{
		angleOfZRotation = angleOfZRotation + 0.1f;
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	if (keyPressed == 1)
	{
		glRotatef(angleOfXRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = angleOfXRotation;
		lightPosition[2] = 0.0f;
	}
	else if (keyPressed == 2)
	{
		glRotatef(angleOfYRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = angleOfYRotation;
	}
	else if (keyPressed == 3)
	{
		glRotatef(angleOfZRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = angleOfZRotation;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	draw24Spheres();

	SwapBuffers(ghdc);
}

void draw24Spheres()
{
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShininess[1];

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// ***** 1st sphere on 1st column, emerald *****
	materialAmbient[0] = { 0.0215f };
	materialAmbient[1] = { 0.1745f };
	materialAmbient[2] = { 0.0215f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.07568f };
	materialDiffuse[1] = { 0.61424f };
	materialDiffuse[2] = { 0.07568f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.633f };
	materialSpecular[1] = { 0.727811f };
	materialSpecular[2] = { 0.633f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.6f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 14.0f, 0.0f);
	gluSphere(Quadric[0], 1.0f, 30, 30);

	// ***** 2nd sphere on 1st column, jade *****
	materialAmbient[0] = { 0.135f };
	materialAmbient[1] = { 0.2225f };
	materialAmbient[2] = { 0.1575f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.54f };
	materialDiffuse[1] = { 0.89f };
	materialDiffuse[2] = { 0.63f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.316228f };
	materialSpecular[1] = { 0.316228f };
	materialSpecular[2] = { 0.316228f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.1f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 11.5f, 0.0f);
	gluSphere(Quadric[1], 1.0f, 30, 30);

	// ***** 3rd sphere on 1st column, obsidian *****
	materialAmbient[0] = { 0.05375f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.06625f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.18275f };
	materialDiffuse[1] = { 0.17f };
	materialDiffuse[2] = { 0.22525f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.332741f };
	materialSpecular[1] = { 0.328634f };
	materialSpecular[2] = { 0.346435f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.3f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 9.0f, 0.0f);
	gluSphere(Quadric[2], 1.0f, 30, 30);

	// ***** 4th sphere on 1st column, pearl *****
	materialAmbient[0] = { 0.25f };
	materialAmbient[1] = { 0.20725f };
	materialAmbient[2] = { 0.20725f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 1.0f };
	materialDiffuse[1] = { 0.829f };
	materialDiffuse[2] = { 0.829f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.296648f };
	materialSpecular[1] = { 0.296648f };
	materialSpecular[2] = { 0.296648f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.088f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 6.5f, 0.0f);
	gluSphere(Quadric[3], 1.0f, 30, 30);

	// ***** 5th sphere on 1st column, ruby *****
	materialAmbient[0] = { 0.1745f };
	materialAmbient[1] = { 0.01175f };
	materialAmbient[2] = { 0.01175f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.61424f };
	materialDiffuse[1] = { 0.04136f };
	materialDiffuse[2] = { 0.04136f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.727811f };
	materialSpecular[1] = { 0.626959f };
	materialSpecular[2] = { 0.626959f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.6f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 4.0f, 0.0f);
	gluSphere(Quadric[4], 1.0f, 30, 30);

	// ***** 6th sphere on 1st column, turquoise *****
	materialAmbient[0] = { 0.1f };
	materialAmbient[1] = { 0.18725f };
	materialAmbient[2] = { 0.1745f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.396f };
	materialDiffuse[1] = { 0.74151f };
	materialDiffuse[2] = { 0.69102f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.297254f };
	materialSpecular[1] = { 0.30829f };
	materialSpecular[2] = { 0.306678f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.1f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 1.5f, 0.0f);
	gluSphere(Quadric[5], 1.0f, 30, 30);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ***** 1st sphere on 2nd column, brass *****
	materialAmbient[0] = { 0.329412f };
	materialAmbient[1] = { 0.223529f };
	materialAmbient[2] = { 0.027451f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.780392f };
	materialDiffuse[1] = { 0.568627f };
	materialDiffuse[2] = { 0.113725f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.992157f };
	materialSpecular[1] = { 0.941176f };
	materialSpecular[2] = { 0.807843f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.21794872f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 14.0f, 0.0f);
	gluSphere(Quadric[6], 1.0f, 30, 30);

	// ***** 2nd sphere on 2nd column, bronze *****
	materialAmbient[0] = { 0.2125f };
	materialAmbient[1] = { 0.1275f };
	materialAmbient[2] = { 0.054f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.714f };
	materialDiffuse[1] = { 0.4284f };
	materialDiffuse[2] = { 0.18144f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.393548f };
	materialSpecular[1] = { 0.271906f };
	materialSpecular[2] = { 0.166721f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.2f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 11.5f, 0.0f);
	gluSphere(Quadric[7], 1.0f, 30, 30);

	// ***** 3rd sphere on 2nd column, chrome *****
	materialAmbient[0] = { 0.25f };
	materialAmbient[1] = { 0.25f };
	materialAmbient[2] = { 0.25f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.4f };
	materialDiffuse[1] = { 0.4f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.774597f };
	materialSpecular[1] = { 0.774597f };
	materialSpecular[2] = { 0.774597f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.6f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 9.0f, 0.0f);
	gluSphere(Quadric[8], 1.0f, 30, 30);

	// ***** 4th sphere on 2nd column, copper *****
	materialAmbient[0] = { 0.19125f };
	materialAmbient[1] = { 0.0735f };
	materialAmbient[2] = { 0.0225f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.7038f };
	materialDiffuse[1] = { 0.27048f };
	materialDiffuse[2] = { 0.0828f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.256777f };
	materialSpecular[1] = { 0.1376229f };
	materialSpecular[2] = { 0.086014f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.1f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 6.5f, 0.0f);
	gluSphere(Quadric[9], 1.0f, 30, 30);

	// ***** 5th sphere on 2nd column, gold *****
	materialAmbient[0] = { 0.24725f };
	materialAmbient[1] = { 0.1995f };
	materialAmbient[2] = { 0.0745f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.75164f };
	materialDiffuse[1] = { 0.60648f };
	materialDiffuse[2] = { 0.22648f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.628281f };
	materialSpecular[1] = { 0.555802f };
	materialSpecular[2] = { 0.366065f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.4f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 4.0f, 0.0f);
	gluSphere(Quadric[10], 1.0f, 30, 30);

	// ***** 6th sphere on 2nd column, silver *****
	materialAmbient[0] = { 0.19225f };
	materialAmbient[1] = { 0.19225f };
	materialAmbient[2] = { 0.19225f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.50754f };
	materialDiffuse[1] = { 0.50754f };
	materialDiffuse[2] = { 0.50754f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.508273f };
	materialSpecular[1] = { 0.508273f };
	materialSpecular[2] = { 0.508273f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.4f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 1.5f, 0.0f);
	gluSphere(Quadric[11], 1.0f, 30, 30);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ***** 1st sphere on 3rd column, black *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.01f };
	materialDiffuse[1] = { 0.01f };
	materialDiffuse[2] = { 0.01f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.50f };
	materialSpecular[1] = { 0.50f };
	materialSpecular[2] = { 0.50f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 14.0f, 0.0f);
	gluSphere(Quadric[12], 1.0f, 30, 30);

	// ***** 2nd sphere on 3rd column, cyan *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.1f };
	materialAmbient[2] = { 0.06f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.0f };
	materialDiffuse[1] = { 0.50980392f };
	materialDiffuse[2] = { 0.50980392f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.50196078f };
	materialSpecular[1] = { 0.50196078f };
	materialSpecular[2] = { 0.50196078f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 11.5f, 0.0f);
	gluSphere(Quadric[13], 1.0f, 30, 30);

	// ***** 3rd sphere on 3rd column, green *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.1f };
	materialDiffuse[1] = { 0.35f };
	materialDiffuse[2] = { 0.1f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.45f };
	materialSpecular[1] = { 0.55f };
	materialSpecular[2] = { 0.45f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 9.0f, 0.0f);
	gluSphere(Quadric[14], 1.0f, 30, 30);

	// ***** 4th sphere on 3rd column, red *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.0f };
	materialDiffuse[2] = { 0.0f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.6f };
	materialSpecular[2] = { 0.6f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 6.5f, 0.0f);
	gluSphere(Quadric[15], 1.0f, 30, 30);

	// ***** 5th sphere on 3rd column, white *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.55f };
	materialDiffuse[1] = { 0.55f };
	materialDiffuse[2] = { 0.55f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.70f };
	materialSpecular[1] = { 0.70f };
	materialSpecular[2] = { 0.70f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 4.0f, 0.0f);
	gluSphere(Quadric[16], 1.0f, 30, 30);

	// ***** 6th sphere on 3rd column, yellow plastic *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.0f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.60f };
	materialSpecular[1] = { 0.60f };
	materialSpecular[2] = { 0.50f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 1.5f, 0.0f);
	gluSphere(Quadric[17], 1.0f, 30, 30);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ***** 1st sphere on 4th column, black *****
	materialAmbient[0] = { 0.02f };
	materialAmbient[1] = { 0.02f };
	materialAmbient[2] = { 0.02f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.01f };
	materialDiffuse[1] = { 0.01f };
	materialDiffuse[2] = { 0.01f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.4f };
	materialSpecular[1] = { 0.4f };
	materialSpecular[2] = { 0.4f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 14.0f, 0.0f);
	gluSphere(Quadric[18], 1.0f, 30, 30);

	// ***** 2nd sphere on 4th column, cyan *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.05f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.4f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.5f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.04f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.7f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 11.5f, 0.0f);
	gluSphere(Quadric[19], 1.0f, 30, 30);

	// ***** 3rd sphere on 4th column, green *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.4f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.04f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.04f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 9.0f, 0.0f);
	gluSphere(Quadric[20], 1.0f, 30, 30);

	// ***** 4th sphere on 4th column, red *****
	materialAmbient[0] = { 0.05f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.4f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.04f };
	materialSpecular[2] = { 0.04f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125 * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 6.5f, 0.0f);
	gluSphere(Quadric[21], 1.0f, 30, 30);

	// ***** 5th sphere on 4th column, white *****
	materialAmbient[0] = { 0.05f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.05f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.5f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.7f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 4.0f, 0.0f);
	gluSphere(Quadric[22], 1.0f, 30, 30);

	// ***** 6th sphere on 4th column, yellow rubber *****
	materialAmbient[0] = { 0.05f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.04f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 1.5f, 0.0f);
	gluSphere(Quadric[23], 1.0f, 30, 30);
}

void uninitialize(void)
{
	if (gbFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	if (gpfile)
	{
		fprintf(gpfile, "fclose Successful");
		fclose(gpfile);
		gpfile = NULL;
	}
	if (Quadric)
	{
		for (int i = 0; i < 24; i++)
		{
			gluDeleteQuadric(Quadric[i]);
		}
		Quadric[24];
	}
}

void ToggleFullScreen(void)
{
	MONITORINFO mi;
	if (gbFullScreen == false)
	{
		dwstyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwstyle && WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwstyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
		gbFullScreen = true;
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
		ShowCursor(TRUE);
		gbFullScreen = false;
	}
}


//GLfloat materialAmbient[4][6][4] =
//{
//{   // 1st column
//	{0.0215f, 0.1745f, 0.0215f, 1.0f },//emerald
//	{0.135f, 0.2225f, 0.1575f, 1.0f },//jade
//	{0.05375f, 0.05f, 0.06625f, 1.0f },//obsidian
//	{0.25f, 0.20725f, 0.20725f, 1.0f },//pearl
//	{0.1745f, 0.01175f, 0.01175f, 1.0f },//ruby
//	{0.1f, 0.18725f, 0.1745f, 1.0f }//turquoise
//},
//{   // 2nd column
//	{0.329412f, 0.223529f, 0.027451f, 1.0f },//brass
//	{0.2125f, 0.1275f, 0.054f, 1.0f },//bronze
//	{0.25f, 0.25f, 0.25f, 1.0f },//chrome
//	{0.19125f, 0.0735f, 0.0225f, 1.0f },//copper
//	{0.24725f, 0.1995f, 0.0745f, 1.0f },//gold
//	{0.19225f, 0.19225f, 0.19225f, 1.0f }//silver
//},
//{   // 3rd column
//	{0.0f, 0.0f, 0.0f, 1.0f },//black
//	{0.0f, 0.1f, 0.06f, 1.0f },//cyan
//	{0.0f, 0.0f, 0.0f, 1.0f },//green
//	{0.0f, 0.0f, 0.0f, 1.0f },//red
//	{0.0f, 0.0f, 0.0f, 1.0f },//white
//	{0.0f, 0.0f, 0.0f, 1.0f }//yellow plastic
//},
//{   // 4th column
//	{0.02f, 0.02f, 0.02f, 1.0f },//black
//	{0.0f, 0.05f, 0.05f, 1.0f },//cyan
//	{0.0f, 0.05f, 0.0f, 1.0f },//green
//	{0.05f, 0.0f, 0.0f, 1.0f },//red
//	{0.05f, 0.05f, 0.05f, 1.0f },//white
//	{0.05f, 0.05f, 0.0f, 1.0f }//yellow rubber
//}
//};

//GLfloat materialDiffuse[4][6][4] =
//{
//	{   // 1st column
//		{0.07568f, 0.61424f, 0.07568f, 1.0f},//emerald
//		{0.54f, 0.89f, 0.63f, 1.0f},//jade
//		{0.18275f, 0.17f, 0.22525f, 1.0f},//obsidian
//		{1.0f, 0.829f, 0.829f, 1.0f},//pearl
//		{0.61424f, 0.04136f, 0.04136f, 1.0f},//ruby
//		{0.396f, 0.74151f, 0.69102f, 1.0f},//turquoise
//	},
//	{   // 2nd column
//		{0.780392f, 0.568627f, 0.113725f, 1.0f},//brass
//		{0.714f, 0.4284f, 0.18144f, 1.0f},//bronze
//		{0.4f, 0.4f, 0.4f, 1.0f},//chrome
//		{0.7038f, 0.27048f, 0.0828f, 1.0f},//copper
//		{0.75164f, 0.60648f, 0.22648f, 1.0f},//gold
//		{0.50754f, 0.50754f, 0.50754f, 1.0f},//silver
//	},
//	{   // 3rd column
//		{0.01f, 0.01f, 0.01f, 1.0f},//black
//		{0.0f, 0.50980392f, 0.50980392f, 1.0f},//cyan
//		{0.1f, 0.35f, 0.1f, 1.0f},//green
//		{0.5f, 0.0f, 0.0f, 1.0f},//red
//		{0.55f, 0.55f, 0.55f, 1.0f},//white
//		{0.5f, 0.5f, 0.0f, 1.0f},//yellow plastic
//	},
//	{   // 4th column
//		{0.01f, 0.01f, 0.01f, 1.0f},//black
//		{0.4f, 0.5f, 0.5f, 1.0f},//cyan
//		{0.4f, 0.5f, 0.4f, 1.0f},//green
//		{0.5f, 0.4f, 0.4f, 1.0f},//red
//		{0.5f, 0.5f, 0.5f, 1.0f},//white
//		{0.5f, 0.5f, 0.4f, 1.0f},//yellow rubber
//	},
//};

//GLfloat materialSpecular[4][6][4] =
//{
//	{   // 1st column
//		{0.633f, 0.727811f, 0.633f, 1.0f},//emerald
//		{0.316228f, 0.316228f, 0.316228f, 1.0f},//jade
//		{0.332741f, 0.328634f, 0.346435f, 1.0f},//obsidian
//		{0.296648f, 0.296648f, 0.296648f, 1.0f},//pearl
//		{0.727811f, 0.626959f, 0.626959f, 1.0f},//ruby
//		{0.297254f, 0.30829f, 0.306678f, 1.0f},//turquoise
//	},
//	{   // 2nd column
//		{0.992157f, 0.941176f, 0.807843f, 1.0f},//brass
//		{0.393548f, 0.271906f, 0.166721f, 1.0f},//bronze
//		{0.774597f, 0.774597f, 0.774597f, 1.0f},//chrome
//		{0.256777f, 0.137622f, 0.086014f, 1.0f},//copper
//		{0.628281f, 0.555802f, 0.366065f, 1.0f},//gold
//		{0.508273f, 0.508273f, 0.508273f, 1.0f},//silver
//	},
//	{   // 3rd column
//		{0.50f, 0.50f, 0.50f, 1.0f},//black
//		{0.50196078f, 0.50196078f, 0.50196078f, 1.0f},//cyan
//		{0.45f, 0.55f, 0.45f, 1.0f},//green
//		{0.7f, 0.6f, 0.6f, 1.0f},//red
//		{0.70f, 0.70f, 0.70f, 1.0f},//white
//		{0.60f, 0.60f, 0.50f, 1.0f},//yellow plastic
//	},
//	{   // 4th column
//		{0.4f, 0.4f, 0.4f, 1.0f},//black
//		{0.04f, 0.7f, 0.7f, 1.0f},//cyan
//		{0.04f, 0.7f, 0.04f, 1.0f},//green
//		{0.7f, 0.04f, 0.04f, 1.0f},//red
//		{0.7f, 0.7f, 0.7f, 1.0f},//white
//		{0.7f, 0.7f, 0.04f, 1.0f},//yellow rubber
//	}
//};

//GLfloat materialShininess[4][6][1] = 
//{
//	{   // 1st column
//		{0.6f * 128.0f},//emerald
//		{0.1f * 128.0f},//jade
//		{0.3f * 128.0f},//obsidian
//		{0.088f * 128.0f},//pearl
//		{0.6f * 128.0f},//ruby
//		{0.1f * 128.0f},//turquoise
//	},
//	{   // 2nd column
//		{0.21794872f * 128.0f},//brass
//		{0.2f * 128.0f},//bronze
//		{0.6f * 128.0f},//chrome
//		{0.1f * 128.0f},//copper
//		{0.4f * 128.0f},//gold
//		{0.4f * 128.0f},//silver
//	},
//	{   // 3rd column
//		{0.25f * 128.0f},//black
//		{0.25f * 128.0f},//cyan
//		{0.25f * 128.0f},//green
//		{0.25f * 128.0f},//red
//		{0.25f * 128.0f},//white
//		{0.25f * 128.0f},//yellow plastic
//	},
//	{   // 4th column
//		{0.078125f * 128.0f},//black
//		{0.078125f * 128.0f},//cyan
//		{0.078125f * 128.0f},//green
//		{0.078125f * 128.0f},//red
//		{0.078125f * 128.0f},//white
//		{0.078125f * 128.0f},//yellow rubber
//	}
//};
