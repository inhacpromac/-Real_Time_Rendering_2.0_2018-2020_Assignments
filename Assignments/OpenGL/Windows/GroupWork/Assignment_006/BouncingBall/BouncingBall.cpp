// Assignment_005
// Bouncing Ball

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
GLfloat spinning_angle = 0.0f;

void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();
void drawGround();
void drawBall();

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

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
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

int moveBall = 0;
GLfloat posBallX = -0.9f;
GLfloat posBallY = 0.5f;
GLfloat scaleBallY = 1.0f;

void display(void)
{
	//conditions
	if (moveBall == 0 && posBallX >= 0.0f && posBallY >= -0.4f)
	{
		moveBall = 1;
	}
	if (moveBall == 1 && scaleBallY <= 0.5f && posBallY >= -0.8f)
	{
		moveBall = 2;
	}
	if (moveBall == 2 && scaleBallY >= 0.0f && posBallY >= -0.4f)
	{
		moveBall = 3;
	}
	if (moveBall == 3 && posBallX >= 0.9f && posBallY >= 0.5f)
	{
		moveBall = 4;
	}

	//movement
	if (moveBall != 1 && moveBall < 1)
	{
		posBallX = posBallX + 0.0001f;
		posBallY = posBallY - 0.0001f;
	}
	if (moveBall == 1 && moveBall != 2 && moveBall < 2)
	{
		scaleBallY -= 0.0009f;
		posBallY = posBallY - 0.0001f;
	}
	if (moveBall == 2 && moveBall != 3 && moveBall < 3)
	{
		scaleBallY += 0.0009f;
		posBallY = posBallY + 0.0001f;
	}
	if (moveBall == 3 && moveBall != 4 && moveBall < 4)
	{
		posBallX = posBallX + 0.001f;
		posBallY = posBallY + 0.001f;
	}


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -2.0f);

	drawGround();

	glLoadIdentity();
	glTranslatef(posBallX, posBallY, -2.0f);
	glScalef(1.0f, scaleBallY, 1.0f);

	drawBall();

	SwapBuffers(ghdc);
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
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
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

void drawGround()
{
	glLineWidth(3.0f);
	glBegin(GL_LINES);

	glColor3f(0.4275f, 0.3451f, 0.2902f);
	glVertex3f(-1.0f, -0.5f, 0.0f);
	glVertex3f(1.0f, -0.5f, 0.0f);

	glEnd();
}

GLfloat angle = 0.0f;

void drawBall()
{
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle < 2 * M_PI; angle += 0.01f)
	{
		glVertex3f((GLfloat)cos(angle)*0.1f, (GLfloat)sin(angle)*0.1f, 0.0f);
	}

	glEnd();
}