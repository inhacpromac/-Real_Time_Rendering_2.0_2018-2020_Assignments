// Assignment_039
// Custom Matrix

// header
#include<windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

#define WIN_WIDTH 700
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

void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();
void drawTriangle();

GLfloat Identity_Matrix[16];
GLfloat Translate_Matrix[16];
GLfloat Rotate_Matrix_X[16];
GLfloat Rotate_Matrix_Y[16];
GLfloat Rotate_Matrix_Z[16];
GLfloat tx=0.0f, ty=0.0f, tz=0.0f;
GLfloat theta = 0.0f;

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
	gluPerspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

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

void drawTriangle()
{
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, -0.5f);

	glEnd();
}