// Assignment_042
// Dynamic_India

// header
#include<windows.h>
#include<stdio.h>
#include<gl/GL.h>
#include<gl/GLU.h>

#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"winmm.lib")

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

void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();
void drawI();
void drawN();
void drawD();
void drawA();
void drawFlag();

struct Plane
{
	GLfloat top = 0.0f;
	GLfloat bottom = 0.0f;
	GLfloat left = 0.0f;
	GLfloat right = 0.0f;
	//GLfloat negLeft = 0.0f;
	//GLfloat negRight = 0.0f;
	//GLfloat R = 0.0f;
	//GLfloat G = 0.0f;
	//GLfloat B = 0.0f;
};

void drawPlane(struct Plane);

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
	case WM_CREATE:
		PlaySound(TEXT("QQBJ.wav"), NULL, SND_FILENAME | SND_ASYNC);
		break;
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

GLint moveI1 = 0, moveN = 0, moveD = 0, moveA = 0, moveI2 = 0, moveP1 = 0, moveP2 = 0, moveP3 = 0;
GLfloat posI1 = -9.0f;
GLfloat posN = -6.0f;
GLfloat posI2 = 6.0f;
GLfloat posA = 9.0f;
GLfloat posDOR = 0.0f; // red color of D to achive orange color
GLfloat posDOG = 0.0f; // green color of D to achive orange color
GLfloat posDOB = 0.0f; // blue color of D to achive orange color
GLfloat posDGR = 0.0f; // red color of D to achive green color
GLfloat posDGG = 0.0f; // green color of D to achive green color
GLfloat posDGB = 0.0f; // blue color of D to achive green color
GLfloat posP1X = -9.0f;
GLfloat posP1Y = 6.0f;
GLfloat posP2X = -9.0f;
GLfloat posP2Y = 0.5f;
GLfloat posP3X = -9.0f;
GLfloat posP3Y = -5.0f;
GLfloat posFlag = -101.0f;

void display(void)
{
	//Conditions
	if (moveI1 == 0 && posI1 >= -3.5f)
	{
		moveI1 = 1;
	}
	if (moveA == 0 && posA <= 1.9f)
	{
		moveA = 1;
	}
	if (moveN == 0 && posN >= 0.0f)
	{
		moveN = 1;
	}
	if (moveI2 == 0 && posI2 <= 0.0f)
	{
		moveI2 = 1;
	}
	if (moveD == 0 && posDOR >= 1.0f && posDOG >= 0.5f && posDGG >= 1.0f)
	{
		moveD = 1;
	}


	//Phase1
	if (moveP1 == 0 && posP1X >= -3.5f && posP1Y <= 0.5f)
	{
		moveP1 = 1;
	}
	if (moveP2 == 0 && posP2X >= -3.5f)
	{
		moveP2 = 1;
	}
	if (moveP3 == 0 && posP3X >= -3.5f && posP3Y >= 0.5f)
	{
		moveP3 = 1;
	}


	//Phase2
	if (moveP1 == 1 && posP1X >= 3.3f && posP1Y <= 0.5f)
	{
		moveP1 = 2;
		posFlag = -10.0f;
	}
	if (moveP2 == 1 && posP2X >= 3.3f)
	{
		moveP2 = 2;
	}
	if (moveP3 == 1 && posP3X >= 3.3f && posP3Y >= 0.5f)
	{
		moveP3 = 2;
	}


	//Phase3
	if (moveP1 == 2 && posP1X >= 9.0f && posP1Y <= 0.5f)
	{
		moveP1 = 3;
	}
	if (moveP2 == 2 && posP2X >= 9.0f)
	{
		moveP2 = 3;
	}
	if (moveP3 == 2 && posP3X >= 9.0f && posP3Y >= 0.5f)
	{
		moveP3 = 3;
	}


	//Movement
	if (moveI1 != 1)
	{
		posI1 = posI1 + 0.001f;
	}
	if (moveI1 == 1 && moveA != 1)
	{
		posA = posA - 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN != 1)
	{
		posN = posN + 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 != 1)
	{
		posI2 = posI2 - 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD != 1)
	{
		posDOR = posDOR + 0.001f;
		posDOG = posDOG + 0.0005f;  // As we need to reach the value of 0.5f we need to increment accordingly. If we dont do this then we get Yellow Color
		posDGG = posDGG + 0.001f;
	}


	//Phase1
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP1 != 1 && moveP1 < 1)
	{
		posP1X = posP1X + 0.001f;
		posP1Y = posP1Y - 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP2 != 1 && moveP2 < 1)
	{
		posP2X = posP2X + 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP3 != 1 && moveP3 < 1)
	{
		posP3X = posP3X + 0.001f;
		posP3Y = posP3Y + 0.001f;
	}
	
	
	//Phase2
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP1 != 2 && moveP1 >= 1 && moveP1 < 2)
	{
		posP1X = posP1X + 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP2 != 2 && moveP1 >= 1 && moveP2 < 2)
	{
		posP2X = posP2X + 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP3 != 2 && moveP1 >= 1 && moveP3 < 2)
	{
		posP3X = posP3X + 0.001f;
	}
	
	
	//Phase3
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP1 != 3 && moveP1 >= 2 && moveP1 < 3)
	{
		posP1X = posP1X + 0.001f;
		posP1Y = posP1Y + 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP2 != 3 && moveP1 >= 2 && moveP2 < 3)
	{
		posP2X = posP2X + 0.001f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP3 != 3 && moveP1 >= 2 && moveP3 < 3)
	{
		posP3X = posP3X + 0.001f;
		posP3Y = posP3Y - 0.001f;
	}
	

	struct Plane Plane1;
	Plane1.top = 0.0f;
	Plane1.bottom = -1.0f;
	Plane1.left = 0.0f;
	Plane1.right = 1.5f;


	struct Plane Plane2;
	Plane2.top = 0.0f;
	Plane2.bottom = -1.0f;
	Plane2.left = 0.0f;
	Plane2.right = 1.5f;

	struct Plane Plane3;
	Plane3.top = 0.0f;
	Plane3.bottom = -1.0f;
	Plane3.left = 0.0f;
	Plane3.right = 1.5f;	

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(posI1, 0.0f, -10.0f);

	drawI();

	glLoadIdentity();
	glTranslatef(-2.75f, posN, -10.0f);

	drawN();

	glLoadIdentity();
	glTranslatef(-0.6f, 0.0f, -10.0f);

	drawD();

	glLoadIdentity();
	glTranslatef(1.15f, posI2, -10.0f);

	drawI();

	glLoadIdentity();
	glTranslatef(posA, 0.0f, -10.0f);

	drawA();

	glLoadIdentity();
	glTranslatef(1.9f, 0.0f, posFlag);

	drawFlag();

	glLoadIdentity();
	glTranslatef(posP1X, posP1Y, -10.0f);

	drawPlane(Plane1);

	glLoadIdentity();
	glTranslatef(posP3X, posP3Y, -10.0f);

	drawPlane(Plane3);

	glLoadIdentity();
	glTranslatef(posP2X, posP2Y, -10.0f);

	drawPlane(Plane2);	

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

void drawI()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);
	glEnd();
}

void drawN()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 0.6f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.8f, -1.5f);

	/*glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -0.6f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.8f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.8f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.8f, 1.5f);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.4f, 0.0f);*/

	glEnd();
}

void drawD()
{
	glBegin(GL_POLYGON);

	glColor3f(posDOR, posDOG, posDOB);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.5f);

	glColor3f(posDGR, posDGG, posDGB);
	glVertex2f(0.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);	

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(posDGR, posDGG, posDGB);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);
	glVertex2f(0.4f, -1.1f);
	glVertex2f(1.0f, -1.1f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(posDGR, posDGG, posDGB);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(1.4f, -1.1f);

	glColor3f(posDOR, posDOG, posDOB);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(posDOR, posDOG, posDOB);
	glVertex2f(1.0f, 1.5f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.4f, 1.1f);
	glVertex2f(1.0f, 1.1f);

	glEnd();
}

void drawA()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.1f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.1f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(1.4f, -1.5f);

	glEnd();
}

void drawPlane(struct Plane P)
{
	// Orange
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(P.left, P.top - 0.35f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.35f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.45f);
	
	//glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(P.left, P.top - 0.45f);

	glEnd();

	// White
	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(P.left, P.top - 0.45f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.45f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.55f);
	
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(P.left, P.top - 0.55f);

	glEnd();

	// Green
	glBegin(GL_QUADS);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(P.left, P.top - 0.55f);

	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.55f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.65f);
	
	//glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(P.left, P.top - 0.65f);

	glEnd();

	// Plane
	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 0.1f, P.top - 0.35f);
	glVertex2f(P.left, P.top - 0.35f);
	glVertex2f(P.left, P.top - 0.65f);
	glVertex2f(P.left + 0.1f, P.top - 0.65f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 0.8f, P.top - 0.25f);
	glVertex2f(P.left + 0.1f, P.top);
	glVertex2f(P.left + 0.1f, P.bottom);
	glVertex2f(P.left + 0.8f, P.top - 0.75f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 1.0f, P.top - 0.25f);
	glVertex2f(P.left + 0.8f, P.top - 0.25f);
	glVertex2f(P.left + 0.8f, P.top - 0.75f);
	glVertex2f(P.left + 1.0f, P.top - 0.75f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 1.25f, P.top - 0.4f);
	glVertex2f(P.left + 1.0f, P.top - 0.4f);
	glVertex2f(P.left + 1.0f, P.top - 0.6f);
	glVertex2f(P.left + 1.25f, P.top - 0.6f);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.right, P.top - 0.5f);
	glVertex2f(P.left + 1.25f, P.top - 0.4f);
	glVertex2f(P.left + 1.25f, P.top - 0.6f);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.right, P.top - 0.5f);
	glVertex2f(P.left + 1.25f, P.top - 0.4f);
	glVertex2f(P.left + 1.25f, P.top - 0.6f);

	glEnd();

	glLineWidth(4.0f);
	glBegin(GL_LINES);

	glColor3f(0.698f, 0.133f, 0.133f);
	glVertex2f(P.left + 0.25f, P.top - 0.25f);
	glVertex2f(P.left + 0.25f, P.top - 0.75f);

	glEnd();

	glBegin(GL_LINES);

	glColor3f(0.698f, 0.133f, 0.133f);
	glVertex2f(P.left + 0.25f, P.top - 0.25f);
	glVertex2f(P.left + 0.25f, P.top - 0.75f);

	glEnd();
}

void drawFlag()
{
	//OrangeFlag
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.0f, 0.15f);
	glVertex2f(0.4f, 0.15f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(1.0f, 0.05f);

	glEnd();

	//WhiteFlag
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.0f, 0.05f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.4f, -0.05f);
	glVertex2f(1.0f, -0.05f);

	glEnd();

	//GreenFlag
	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -0.05f);
	glVertex2f(0.4f, -0.05f);
	glVertex2f(0.4f, -0.15f);
	glVertex2f(1.0f, -0.15f);

	glEnd();
}