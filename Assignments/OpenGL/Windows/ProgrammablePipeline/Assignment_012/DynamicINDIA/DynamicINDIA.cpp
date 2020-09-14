// Assignment_012
// DynamicINDIA

// header
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<glew.h>
#include<gl/GL.h>
#include "vmath.h"
#include "resource.h"

using namespace vmath;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// callback
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
HWND ghwnd = NULL;
HDC ghdc = NULL;
HGLRC ghrc = NULL; // OpenGL Rendering Context
FILE* gpfile = NULL;
DWORD dwstyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
bool bDone = false;
bool gbActiveWindow = false;
bool gbFullScreen = false;

GLuint gShaderProgramObject;

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0,
};
//vao-shapes
//vbo-color,texture,....
GLuint vao;
GLuint vbo_position;
GLuint vbo_color;
GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

// global function declarations
void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();

float transI1 = -10.0f;
float transN = 15.0f;
float transD = 15.0f;
float transI2 = -15.0f;
float transA = 15.0f;

bool bDrawN = false;
bool bDrawI2 = false;
bool bDrawA = false;
bool bDrawD = false;


GLuint gShaderProgramObjectPlane;

GLuint Texture_Plane;
GLuint samplerUniform;
GLuint vao_cube;
GLuint vbo_position_cube;
GLuint vbo_texture_cube;
GLuint mvpUniformPlane;
bool bDrawPlane = false;
float transPlaneX = -15.0f;


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
	void initializePlane();

	// variable declaration
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	GLenum result;

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

	result = glewInit();
	if (result != GLEW_OK)
	{
		fprintf(gpfile, "Cannot initialize GLEW\n");
		uninitialize();
		exit(0);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Write Vertex Shader Object
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec4 vColor;" \
		"uniform mat4 u_mvp_matrix;" \
		"out vec4 out_color;" \
		"void main(void)" \
		"{" \
		"   gl_Position = u_mvp_matrix * vPosition;" \
		"	out_color = vColor;" \
		"}";
	//Specifying Above code to the VertexShaderObject
	glShaderSource(gVertexShaderObject, 1, (const char**)&vertexShaderSourceCode, NULL);
	//Compile the Vertex Shader
	glCompileShader(gVertexShaderObject);
	//Error Checking for Vertex Shader Compiling
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Vertex Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
	//Define Fragment Shader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Write Fragment Shader Object
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 out_color;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"	FragColor = out_color;" \
		"}";
	//Specifying Above code to the FragmentShaderObject
	glShaderSource(gFragmentShaderObject, 1, (const char**)&fragmentShaderSourceCode, NULL);
	//Compile the fragment Shader
	glCompileShader(gFragmentShaderObject);
	//Error Checking for Fragment Shader Compiling
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Fragment Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
	//Create Shader Program Object
	gShaderProgramObject = glCreateProgram();
	//Attach Vertex Shader to Shader Program
	glAttachShader(gShaderProgramObject, gVertexShaderObject);
	//Attach Fragment Shader to Shader Program
	glAttachShader(gShaderProgramObject, gFragmentShaderObject);
	//preLinking Binding to Vertex Attributes
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	//Link The Shader Program
	glLinkProgram(gShaderProgramObject);
	//Error Checking for Shader Program Linking
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObject, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Shader Program Link Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//postLinking retriving uniform location
	mvpUniform = glGetUniformLocation(gShaderProgramObject, "u_mvp_matrix");
	///////////////////////////////////////////////////////////////////////////////////////////////

	initializePlane();

	const GLfloat Vertices[] =
	{
		-5.0f, -2.0f, 0.0f,
		-5.0f, 2.0f, 0.0f,
		-3.0f, -2.0f, 0.0f,
		-3.0f, 2.0f, 0.0f,
		-3.0f, 2.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, -2.0f, 0.0f,
		-1.0f, 2.0f, 0.0f,
		0.0f, 2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		0.5f, -2.0f, 0.0f,
		0.5f, 2.0f, 0.0f,
		0.0f, -2.0f, 0.0f,
		2.0f, -2.0f, 0.0f,
		2.0f, -2.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		3.0f, -2.0f, 0.0f,
		3.0f, 2.0f, 0.0f,
		4.0f, 2.0f, 0.0f,
		6.0f, 2.0f, 0.0f,
		4.0f, 0.0f, 0.0f,
		6.0f, 0.0f, 0.0f,
		4.0f, -2.0f, 0.0f,
		4.0f, 2.0f, 0.0f,
		6.0f, -2.0f, 0.0f,
		6.0f, 2.0f, 0.0f
	};

	const GLfloat Color[] =
	{
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.6f, 0.2f, 0.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.6f, 0.2f, 0.0f
	};

	//Create vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Position
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Color
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Color), Color, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glLineWidth(10.0f);

	perspectiveProjectionMatrix = mat4::identity();

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
	perspectiveProjectionMatrix = perspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

void display(void)
{
	void displayPlane();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(transI1, 0.0f, -20.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_triangle
	glBindVertexArray(vao);

	//simillar Bind with Texture if any

	//Draw necessary Scence
	glDrawArrays(GL_LINES, 0, 2);

	//Unbind with vao_triangle
	glBindVertexArray(0);



	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(-1.0f, transN, -20.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_triangle
	glBindVertexArray(vao);

	//simillar Bind with Texture if any

	//Draw necessary Scence
	glDrawArrays(GL_LINES, 2, 7);

	//Unbind with vao_triangle
	glBindVertexArray(0);



	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(1.0f, transI2, -20.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_triangle
	glBindVertexArray(vao);

	//simillar Bind with Texture if any

	//Draw necessary Scence
	glDrawArrays(GL_LINES, 16, 2);

	//Unbind with vao_triangle
	glBindVertexArray(0);




	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(transA, 0.0f, -20.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_triangle
	glBindVertexArray(vao);

	//simillar Bind with Texture if any

	//Draw necessary Scence
	glDrawArrays(GL_LINES, 18, 8);

	//Unbind with vao_triangle
	glBindVertexArray(0);



	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(0.0f, 0.0f, transD);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_triangle
	glBindVertexArray(vao);

	//simillar Bind with Texture if any

	//Draw necessary Scence
	glDrawArrays(GL_LINES, 8, 8);

	//Unbind with vao_triangle
	glBindVertexArray(0);



	glUseProgram(0);

//	if (bDrawPlane == true)
	{
		displayPlane();
	}


	SwapBuffers(ghdc);


	transI1 += 0.1f;
	if (transI1 >= -1.0f)
	{
		bDrawN = true;
		transI1 = -1.0f;
	}
	if (bDrawN == true)
	{
		transN -= 0.1f;
		if (transN < 0.0f)
		{
			bDrawI2 = true;
			transN = 0.0f;
		}
	}
	if (bDrawI2 == true)
	{
		transI2 += 0.1f;
		if (transI2 > 0.0f)
		{
			bDrawA = true;
			transI2 = 0.0f;
		}
	}
	if (bDrawA == true)
	{
		transA -= 0.1f;
		if (transA < 2.0f)
		{
			bDrawD = true;
			transA = 2.0f;
		}
	}
	if (bDrawD == true)
	{
		transD -= 0.1f;
		if (transD < -20.0f)
		{
			bDrawPlane = true;
			transD = -20.0f;
		}
	}
}

void uninitialize(void)
{
	if (vbo_color)
	{
		glDeleteBuffers(1, &vbo_color);
	}
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
	}
	if (vao)
	{
		glDeleteBuffers(1, &vao);
	}
	if (gShaderProgramObject)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObject);

		//Ask Program: How many Programs are attched to you?
		glGetProgramiv(gShaderProgramObject, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders = (GLuint*)malloc(2 * sizeof(shaderCount));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObject, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//Detach everyone
				glDetachShader(gShaderProgramObject, pShaders[shaderNumber]);

				//Delete The Detached Shaders
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObject);
		gShaderProgramObject = 0;
		glUseProgram(0);
	}
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

void initializePlane(void)
{
	BOOL loadTexture(GLuint * Texture, TCHAR imageResourceID[]);

	GLuint gVertexShaderObject;
	GLuint gFragmentShaderObject;
	GLenum result;

	///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Write Vertex Shader Object
	const GLchar* vertexShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec2 vTexCoord;" \
		"uniform mat4 u_mvp_matrix_plane;" \
		"out vec2 out_texcoord;" \
		"void main(void)" \
		"{" \
		"   gl_Position = u_mvp_matrix_plane * vPosition;" \
		"	out_texcoord = vTexCoord;" \
		"}";
	//Specifying Above code to the VertexShaderObject
	glShaderSource(gVertexShaderObject, 1, (const char**)&vertexShaderSourceCode, NULL);
	//Compile the Vertex Shader
	glCompileShader(gVertexShaderObject);
	//Error Checking for Vertex Shader Compiling
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Vertex Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
	//Define Fragment Shader Object
	gFragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	//Write Fragment Shader Object
	const GLchar* fragmentShaderSourceCode =
		"#version 450 core" \
		"\n" \
		"in vec2 out_texcoord;" \
		"uniform sampler2D u_sampler;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"	/*FragColor = texture(u_sampler, out_texcoord);*/" \
		"	FragColor = vec4(1.0, 0.0, 0.0, 1.0);" \
		"}";
	//Specifying Above code to the FragmentShaderObject
	glShaderSource(gFragmentShaderObject, 1, (const char**)&fragmentShaderSourceCode, NULL);
	//Compile the fragment Shader
	glCompileShader(gFragmentShaderObject);
	//Error Checking for Fragment Shader Compiling
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Fragment Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
	//Create Shader Program Object
	gShaderProgramObjectPlane = glCreateProgram();
	//Attach Vertex Shader to Shader Program
	glAttachShader(gShaderProgramObjectPlane, gVertexShaderObject);
	//Attach Fragment Shader to Shader Program
	glAttachShader(gShaderProgramObjectPlane, gFragmentShaderObject);
	//preLinking Binding
	glBindAttribLocation(gShaderProgramObjectPlane, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPlane, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
	//Link The Shader Program
	glLinkProgram(gShaderProgramObjectPlane);
	//Error Checking for Shader Program Linking
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObjectPlane, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPlane, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPlane, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Shader Program Link Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//postLinking retriving uniform location
	mvpUniformPlane = glGetUniformLocation(gShaderProgramObjectPlane, "u_mvp_matrix_plane");
	samplerUniform = glGetUniformLocation(gShaderProgramObjectPlane, "u_sampler");
	///////////////////////////////////////////////////////////////////////////////////////////////

	const GLfloat cubeVertices[] = {

		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f

	};

	const GLfloat cubeTexCoord[] = {

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f

	};


	//Create vao_cube
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);
	//Position
	glGenBuffers(1, &vbo_position_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Texture
	glGenBuffers(1, &vbo_texture_cube);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture_cube);

	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoord), cubeTexCoord, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDisable(GL_CULL_FACE);

	glEnable(GL_TEXTURE_2D);
	loadTexture(&Texture_Plane, MAKEINTRESOURCE(IDBITMAP_PLANE));
}

BOOL loadTexture(GLuint* Texture, TCHAR imageResourceID[])
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
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bmp.bmWidth, bmp.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bmp.bmBits);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		DeleteObject(hBitmap);
	}
	return(bStatus);
}

void displayPlane()
{
	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;
	mat4 translationMatrix;


	//Initialize above changed Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translationMatrix = mat4::identity();

	//Do necessary Trasformation
	translationMatrix = translate(0.0f/*transPlaneX*/, 0.0f, -20.0f);

	//Do necessary Matrix Multiplication
	modelViewMatrix = modelViewMatrix * translationMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniformPlane, 1, GL_FALSE, modelViewProjectionMatrix);

	glUseProgram(gShaderProgramObjectPlane);

	//Work with texture [A][B][U]
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_Plane);
	glUniform1i(samplerUniform, 0);

	//Bind with vao_cube
	glBindVertexArray(vao_cube);

	//Draw necessary Scence
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	//Unbind with vao_cube
	glBindVertexArray(0);

	glUseProgram(0);

	transPlaneX += 0.1f;
	
}

