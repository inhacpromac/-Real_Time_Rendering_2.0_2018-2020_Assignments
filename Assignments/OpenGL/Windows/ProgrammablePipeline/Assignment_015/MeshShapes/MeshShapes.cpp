// Assignment_015
// MeshShapes

// header
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<glew.h>
#include<gl/GL.h>
#include "vmath.h"

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

GLuint vbo_point_element;
int giPointElements = 0;

GLuint vbo_lines_element;
int giLinesElements = 0;

GLuint vbo_square_element;
int giSquareElements = 0;

GLuint vbo_diagonal_square_element;
int giDigonalSquareElements = 0;

GLuint vbo_fan_element;
int giFanElements = 0;

GLuint vao_color_square;
GLuint vbo_color_square;
GLuint vbo_color_square_element;
int giColorSquareElements = 0;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;


// global function declarations
void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();

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

	const GLfloat vertices[] =
	{
		-1.0f, 1.0f, 0.0f,
		-0.5f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.5f, 1.0f, 0.0f,

		-1.0f, 0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,

		-1.0f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		0.0f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f
	};

	const GLfloat squareColor[] =
	{
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f,
	};

	GLfloat color[48];
	
	/*color[0] = 1.0f;
	color[1] = 1.0f;
	color[2] = 1.0f;
	color[3] = 1.0f;
	color[4] = 1.0f;
	color[5] = 1.0f;
	color[6] = 1.0f;
	color[7] = 1.0f;
	color[8] = 1.0f;
	color[9] = 1.0f;
	color[10] = 1.0f;
	color[11] = 1.0f;
	color[12] = 1.0f;
	color[13] = 1.0f;
	color[14] = 1.0f;
	color[15] = 1.0f;
	color[16] = 1.0f;
	color[17] = 1.0f;
	color[18] = 1.0f;
	color[19] = 1.0f;
	color[20] = 1.0f;
	color[21] = 1.0f;
	color[22] = 1.0f;
	color[23] = 1.0f;
	color[24] = 1.0f;
	color[25] = 1.0f;
	color[26] = 1.0f;
	color[27] = 1.0f;
	color[28] = 1.0f;
	color[29] = 1.0f;
	color[30] = 1.0f;
	color[31] = 1.0f;
	color[32] = 1.0f;
	color[33] = 1.0f;
	color[34] = 1.0f;
	color[35] = 1.0f;
	color[36] = 1.0f;
	color[37] = 1.0f;
	color[38] = 1.0f;
	color[39] = 1.0f;
	color[40] = 1.0f;
	color[41] = 1.0f;
	color[42] = 1.0f;
	color[43] = 1.0f;
	color[44] = 1.0f;
	color[45] = 1.0f;
	color[46] = 1.0f;
	color[47] = 1.0f;*/

	//Create vao

	for (int i = 0; i < 48; i++)
	{
		color[i] = 1.0f;
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//Position of vertices
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Points
	int pointIndices[16];
	giPointElements = 16;
	for (int iItr = 0; iItr < 16; iItr++)
	{
		pointIndices[iItr] = iItr;
	}
	glGenBuffers(1, &vbo_point_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_point_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pointIndices), pointIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Lines
	int lineIndices[30];
	int iIndex = 0;
	lineIndices[iIndex++] = 0;
	lineIndices[iIndex++] = 12;
	lineIndices[iIndex++] = 1;
	lineIndices[iIndex++] = 13;
	lineIndices[iIndex++] = 2;
	lineIndices[iIndex++] = 14;

	lineIndices[iIndex++] = 0;
	lineIndices[iIndex++] = 3;
	lineIndices[iIndex++] = 4;
	lineIndices[iIndex++] = 7;
	lineIndices[iIndex++] = 8;
	lineIndices[iIndex++] = 11;

	lineIndices[iIndex++] = 4;
	lineIndices[iIndex++] = 1;
	lineIndices[iIndex++] = 8;
	lineIndices[iIndex++] = 2;
	lineIndices[iIndex++] = 12;
	lineIndices[iIndex++] = 3;
	lineIndices[iIndex++] = 13;
	lineIndices[iIndex++] = 7;
	lineIndices[iIndex++] = 14;
	lineIndices[iIndex++] = 11;

	giLinesElements = iIndex;
	glGenBuffers(1, &vbo_lines_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_lines_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lineIndices), lineIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Squares
	int squareIndices[30];
	iIndex = 0;
	squareIndices[iIndex++] = 0;
	squareIndices[iIndex++] = 12;
	squareIndices[iIndex++] = 1;
	squareIndices[iIndex++] = 13;
	squareIndices[iIndex++] = 2;
	squareIndices[iIndex++] = 14;
	squareIndices[iIndex++] = 3;
	squareIndices[iIndex++] = 15;

	squareIndices[iIndex++] = 0;
	squareIndices[iIndex++] = 3;
	squareIndices[iIndex++] = 4;
	squareIndices[iIndex++] = 7;
	squareIndices[iIndex++] = 8;
	squareIndices[iIndex++] = 11;
	squareIndices[iIndex++] = 12;
	squareIndices[iIndex++] = 15;

	giSquareElements = iIndex;
	glGenBuffers(1, &vbo_square_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_square_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Square with Diagonal lines
	int diagonalSquareIndices[30];
	iIndex = 0;
	diagonalSquareIndices[iIndex++] = 0;
	diagonalSquareIndices[iIndex++] = 12;
	diagonalSquareIndices[iIndex++] = 1;
	diagonalSquareIndices[iIndex++] = 13;
	diagonalSquareIndices[iIndex++] = 2;
	diagonalSquareIndices[iIndex++] = 14;
	diagonalSquareIndices[iIndex++] = 3;
	diagonalSquareIndices[iIndex++] = 15;

	diagonalSquareIndices[iIndex++] = 0;
	diagonalSquareIndices[iIndex++] = 3;
	diagonalSquareIndices[iIndex++] = 4;
	diagonalSquareIndices[iIndex++] = 7;
	diagonalSquareIndices[iIndex++] = 8;
	diagonalSquareIndices[iIndex++] = 11;
	diagonalSquareIndices[iIndex++] = 12;
	diagonalSquareIndices[iIndex++] = 15;

	diagonalSquareIndices[iIndex++] = 4;
	diagonalSquareIndices[iIndex++] = 1;
	diagonalSquareIndices[iIndex++] = 8;
	diagonalSquareIndices[iIndex++] = 2;
	diagonalSquareIndices[iIndex++] = 12;
	diagonalSquareIndices[iIndex++] = 3;
	diagonalSquareIndices[iIndex++] = 13;
	diagonalSquareIndices[iIndex++] = 7;
	diagonalSquareIndices[iIndex++] = 14;
	diagonalSquareIndices[iIndex++] = 11;

	giDigonalSquareElements = iIndex;
	glGenBuffers(1, &vbo_diagonal_square_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_diagonal_square_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(diagonalSquareIndices), diagonalSquareIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Fan
	int fanIndices[30];
	iIndex = 0;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 12;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 13;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 14;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 15;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 11;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 7;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 3;
	fanIndices[iIndex++] = 12;
	fanIndices[iIndex++] = 15;
	fanIndices[iIndex++] = 15;
	fanIndices[iIndex++] = 3;

	giFanElements = iIndex;
	glGenBuffers(1, &vbo_fan_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_fan_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fanIndices), fanIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Color
	glGenBuffers(1, &vbo_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);

	glBufferData(GL_ARRAY_BUFFER, sizeof(color), color, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);

	//SquaresWithColorVAO
	glGenVertexArrays(1, &vao_color_square);
	glBindVertexArray(vao_color_square);
	//Color Squares
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Colored Squares
	const GLfloat coloredVertices[] =
	{
		1.0f, 1.0f, 0.0f,
		1.0f, 0.5f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, -0.5f, 0.0f,

		0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,

		0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f,

		-0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		-0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,
	};

	const GLfloat coloredSquareColor[] =
	{
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};

	int colorSquareIndices[300];
	iIndex = 0;
	colorSquareIndices[iIndex++] = 16;
	colorSquareIndices[iIndex++] = 20;
	colorSquareIndices[iIndex++] = 21;

	colorSquareIndices[iIndex++] = 21;
	colorSquareIndices[iIndex++] = 16;
	colorSquareIndices[iIndex++] = 17;

	colorSquareIndices[iIndex++] = 17;
	colorSquareIndices[iIndex++] = 21;
	colorSquareIndices[iIndex++] = 22;

	colorSquareIndices[iIndex++] = 22;
	colorSquareIndices[iIndex++] = 17;
	colorSquareIndices[iIndex++] = 18;

	colorSquareIndices[iIndex++] = 18;
	colorSquareIndices[iIndex++] = 22;
	colorSquareIndices[iIndex++] = 23;

	colorSquareIndices[iIndex++] = 23;
	colorSquareIndices[iIndex++] = 18;
	colorSquareIndices[iIndex++] = 19;

	colorSquareIndices[iIndex++] = 15;
	colorSquareIndices[iIndex++] = 11;
	colorSquareIndices[iIndex++] = 10;

	colorSquareIndices[iIndex++] = 10;
	colorSquareIndices[iIndex++] = 15;
	colorSquareIndices[iIndex++] = 14;

	colorSquareIndices[iIndex++] = 14;
	colorSquareIndices[iIndex++] = 10;
	colorSquareIndices[iIndex++] = 9;

	colorSquareIndices[iIndex++] = 9;
	colorSquareIndices[iIndex++] = 14;
	colorSquareIndices[iIndex++] = 13;

	colorSquareIndices[iIndex++] = 13;
	colorSquareIndices[iIndex++] = 9;
	colorSquareIndices[iIndex++] = 8;

	colorSquareIndices[iIndex++] = 8;
	colorSquareIndices[iIndex++] = 13;
	colorSquareIndices[iIndex++] = 12;

	colorSquareIndices[iIndex++] = 4;
	colorSquareIndices[iIndex++] = 0;
	colorSquareIndices[iIndex++] = 1;

	colorSquareIndices[iIndex++] = 1;
	colorSquareIndices[iIndex++] = 4;
	colorSquareIndices[iIndex++] = 5;

	colorSquareIndices[iIndex++] = 5;
	colorSquareIndices[iIndex++] = 1;
	colorSquareIndices[iIndex++] = 2;

	colorSquareIndices[iIndex++] = 2;
	colorSquareIndices[iIndex++] = 5;
	colorSquareIndices[iIndex++] = 6;

	colorSquareIndices[iIndex++] = 6;
	colorSquareIndices[iIndex++] = 2;
	colorSquareIndices[iIndex++] = 3;

	colorSquareIndices[iIndex++] = 3;
	colorSquareIndices[iIndex++] = 6;
	colorSquareIndices[iIndex++] = 7;


	giColorSquareElements = iIndex;

	//SquaresWithColorVAO
	glGenVertexArrays(1, &vao_color_square);
	glBindVertexArray(vao_color_square);

	//Color Squares
	glGenBuffers(1, &vbo_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(coloredVertices), coloredVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_square);

	glBufferData(GL_ARRAY_BUFFER, sizeof(coloredSquareColor), coloredSquareColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &vbo_color_square_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_color_square_element);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(colorSquareIndices), colorSquareIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(gShaderProgramObject);

	//Declaration of Matrices
	mat4 modelViewMatrix;
	mat4 modelViewProjectionMatrix;

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(-3.0f, 1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao
	glBindVertexArray(vao);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glPointSize(2.5f);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_point_element);
	glDrawElements(GL_POINTS, giPointElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(0.0f, 1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao
	glBindVertexArray(vao);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_lines_element);
	glDrawElements(GL_LINES, giLinesElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(3.0f, 1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao
	glBindVertexArray(vao);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_square_element);
	glDrawElements(GL_LINES, giSquareElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(-3.0f, -1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao
	glBindVertexArray(vao);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_diagonal_square_element);
	glDrawElements(GL_LINES, giDigonalSquareElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(0.0f, -1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao
	glBindVertexArray(vao);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_fan_element);
	glDrawElements(GL_LINES, giFanElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(2.5f, -1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_color_square
	glBindVertexArray(vao_color_square);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_color_square_element);
	glDrawElements(GL_TRIANGLES, giColorSquareElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao_color_square
	glBindVertexArray(0);

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();

	//Do necessary Trasformation
	modelViewMatrix = translate(3.0f, -1.0f, -8.0f);

	//Do necessary Matrix Multiplication
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao
	glBindVertexArray(vao);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_square_element);
	glDrawElements(GL_LINES, giSquareElements, GL_UNSIGNED_INT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);
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

