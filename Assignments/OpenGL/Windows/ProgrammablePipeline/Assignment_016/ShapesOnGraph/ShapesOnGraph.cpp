// Assignment_016
// ShapesOnGraph

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
//bool gbTriangle = false;
//bool gbSquare = false;
//bool gbCircle = false;

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
GLuint vao_graph;
GLuint vbo_graph_position;
GLuint vbo_graph_color;

GLuint vao_triangle;
GLuint vbo_position_triangle;
GLuint vbo_color_triangle;

GLuint vao_square;
GLuint vbo_position_square;
GLuint vbo_color_square;

GLuint vao_circle;
GLuint vbo_circle_position;
GLuint vbo_circle_color;

GLuint mvpUniform;
mat4 perspectiveProjectionMatrix;

const int numberOfCircleLines = 5000;
const int numberOfCircleVertices = numberOfCircleLines * 2 * 3;
const int numberOfTriangleVertices = 3 * 3;
const int numberOfSquareVertices = 4 * 3;
const int numberOfGraphVertices = 1024;
// 21 lines with 2 verticle and horizontal 3 line with 2 middle lines = (21*2*3)+2 = 1024
const int PI = 3.14;
int giNumberOfGraphLines = 0;
int giTotalNumberOfLines = 0;

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
		/*case 't':
		case 'T':
			if (gbTriangle == false )
			{
				gbTriangle = true;
				gbCircle = false;
				gbSquare = false;
			}
			else if(gbTriangle == true)
			{
				gbTriangle = false;
				gbCircle = false;
				gbSquare = false;
			}
		case 'c':
		case 'C':
			if (gbCircle == false)
			{
				gbTriangle = false;
				gbCircle = true;
				gbSquare = false;
			}
			else
			{
				gbTriangle = false;
				gbCircle = false;
				gbSquare = false;
			}
		case 's':
		case 'S':
			if (gbSquare == false)
			{
				gbTriangle = false;
				gbCircle = false;
				gbSquare = true;
			}
			else
			{
				gbTriangle = false;
				gbCircle = false;
				gbSquare = false;
			}*/

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
	
	// Graph
	GLfloat graphVertices[numberOfGraphVertices] = { 0 };
	GLfloat graphColor[numberOfGraphVertices] = { 0 };
	int iArrayIndex = 0;
	float fDifference = 1.0f / 20.0f;
	for (float fIterator = -1.0f; fIterator <= 1.05f; fIterator += fDifference)
	{
		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = -1.0f;
		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = 0.0f;

		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = 1.0f;
		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = 0.0f;

		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = -1.0;
		graphVertices[iArrayIndex++] = 0.0f;

		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = 1.0;
		graphVertices[iArrayIndex++] = 0.0f;
	}

	graphColor[iArrayIndex] = 1;
	graphColor[iArrayIndex + 1] = 0;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = -1.0f;
	graphVertices[iArrayIndex++] = 0.0f;
	graphVertices[iArrayIndex++] = 0.0f;

	graphColor[iArrayIndex] = 1;
	graphColor[iArrayIndex + 1] = 0;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = 1.0f;
	graphVertices[iArrayIndex++] = 0.0f;
	graphVertices[iArrayIndex++] = 0.0f;

	graphColor[iArrayIndex] = 0;
	graphColor[iArrayIndex + 1] = 1;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = 0.0f;
	graphVertices[iArrayIndex++] = -1.0f;
	graphVertices[iArrayIndex++] = 0.0f;

	graphColor[iArrayIndex] = 0;
	graphColor[iArrayIndex + 1] = 1;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = 0.0f;
	graphVertices[iArrayIndex++] = 1.0f;
	graphVertices[iArrayIndex++] = 0.0f;

	giNumberOfGraphLines = iArrayIndex / 2;

	// Triangle
	const GLfloat triangleVertices[] =
	{
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f
	};

	const GLfloat triangleColor[] =
	{
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0
	};

	// Square
	const GLfloat squareVertices[] =
	{
		-1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
	};

	const GLfloat squareColor[] =
	{
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0
	};
	
	glLineWidth(2.0f);

	// Circle
	GLfloat circleVertices[numberOfCircleLines * 2 * 3] = { 0 };
	GLfloat circleColor[numberOfCircleLines * 2 * 3] = { 0 };

	iArrayIndex = 0;
	for (int iIterator = 0; iIterator < numberOfCircleLines; iIterator++)
	{
		circleColor[iArrayIndex] = 1.0;
		circleColor[iArrayIndex + 1] = 1.0;
		circleColor[iArrayIndex + 2] = 0.0;
		circleVertices[iArrayIndex++] = (GLfloat)cos((5.0f * PI * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = (GLfloat)sin((5.0f * PI * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = 0.0f;

		iIterator++;

		circleColor[iArrayIndex] = 1.0;
		circleColor[iArrayIndex + 1] = 1.0;
		circleColor[iArrayIndex + 2] = 0.0;
		circleVertices[iArrayIndex++] = (GLfloat)cos((5.0f * PI * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = (GLfloat)sin((5.0f * PI * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = 0.0f;
	}
	
	//Create vao_graph
	glGenVertexArrays(1, &vao_graph);
	glBindVertexArray(vao_graph);
	//Position
	glGenBuffers(1, &vbo_graph_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_graph_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(graphVertices), graphVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Color
	glGenBuffers(1, &vbo_graph_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_graph_color);

	glBufferData(GL_ARRAY_BUFFER, sizeof(graphColor), graphColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Create vao_triangle
	glGenVertexArrays(1, &vao_triangle);
	glBindVertexArray(vao_triangle);
	//Position
	glGenBuffers(1, &vbo_position_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);

	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Color
	glGenBuffers(1, &vbo_color_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);

	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Create vao_square
	glGenVertexArrays(1, &vao_square);
	glBindVertexArray(vao_square);
	//Position
	glGenBuffers(1, &vbo_position_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);

	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Color
	glGenBuffers(1, &vbo_color_square);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color_square);

	glBufferData(GL_ARRAY_BUFFER, sizeof(squareColor), squareColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//Create vao_circle
	glGenVertexArrays(1, &vao_circle);
	glBindVertexArray(vao_circle);
	//Position
	glGenBuffers(1, &vbo_circle_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);

	glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	//Color
	glGenBuffers(1, &vbo_circle_color);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_color);

	glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);

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
	mat4 translateMatrix;
	mat4 scaleMatrix;
	mat4 modelViewProjectionMatrix;

	//////GRAPH//////

	//Initialize above Matrices to identity
	modelViewMatrix = mat4::identity();
	modelViewProjectionMatrix = mat4::identity();
	translateMatrix = mat4::identity();
	scaleMatrix = mat4::identity();

	//Do necessary Trasformation
	translateMatrix = translate(0.0f, 0.0f, -3.0f);

	//Do necessary Matrix Multiplication
	modelViewMatrix = modelViewMatrix * translateMatrix;
	modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_graph
	glBindVertexArray(vao_graph);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glDrawArrays(GL_LINES, 0, giNumberOfGraphLines);

	//Unbind with vao_graph
	glBindVertexArray(0);
	
	//if (gbTriangle == true && gbCircle == false && gbSquare == false)
	//{
		//////TRIANGLE//////

		//Initialize above Matrices to identity
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translateMatrix = mat4::identity();
		scaleMatrix = mat4::identity();

		//Do necessary Trasformation
		translateMatrix = translate(-0.5f, 0.5f, -3.0f);
		scaleMatrix = scale(0.5f, 0.5f, 0.0f);

		//Do necessary Matrix Multiplication
		modelViewMatrix = modelViewMatrix * translateMatrix;
		modelViewMatrix = modelViewMatrix * scaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary Matrix to Shader in respective Uniform
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		//Bind with vao_triangle
		glBindVertexArray(vao_triangle);

		//simillar Bind with Texture if any


		//Draw necessary Scence
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//Unbind with vao_triangle
		glBindVertexArray(0);
	//}

	//if (gbSquare == true && gbTriangle == false && gbCircle == false)
	//{
		//////SQUARE//////

		//Initialize above changed Matrices to identity
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translateMatrix = mat4::identity();
		scaleMatrix = mat4::identity();

		//Do necessary Trasformation
		translateMatrix = translate(0.5f, 0.5f, -3.0f);
		scaleMatrix = scale(0.5f, 0.5f, 0.0f);

		//Do necessary Matrix Multiplication
		modelViewMatrix = modelViewMatrix * translateMatrix;
		modelViewMatrix = modelViewMatrix * scaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary Matrix to Shader in respective Uniform
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		//Bind with vao_square
		glBindVertexArray(vao_square);

		//simillar Bind with Texture if any


		//Draw necessary Scence
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		//Unbind with vao_square
		glBindVertexArray(0);
	//}

	//if (gbCircle == true && gbTriangle == false && gbSquare == false)
	//{
		//////CIRCLE//////

		//Initialize above changed Matrices to identity
		modelViewMatrix = mat4::identity();
		modelViewProjectionMatrix = mat4::identity();
		translateMatrix = mat4::identity();
		scaleMatrix = mat4::identity();

		//Do necessary Trasformation
		translateMatrix = translate(0.0f, -0.5f, -3.0f);
		scaleMatrix = scale(0.5f, 0.5f, 0.5f);

		//Do necessary Matrix Multiplication
		modelViewMatrix = modelViewMatrix * translateMatrix;
		modelViewMatrix = modelViewMatrix * scaleMatrix;
		modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

		//Send necessary Matrix to Shader in respective Uniform
		glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

		//Bind with vao_square
		glBindVertexArray(vao_circle);

		//simillar Bind with Texture if any


		//Draw necessary Scence
		glDrawArrays(GL_LINES, 0, numberOfCircleLines);

		//Unbind with vao_square
		glBindVertexArray(0);

		glUseProgram(0);
	//}

	SwapBuffers(ghdc);
}

void uninitialize(void)
{
	if (vbo_circle_color)
	{
		glDeleteBuffers(1, &vbo_circle_color);
	}
	if (vbo_circle_position)
	{
		glDeleteBuffers(1, &vbo_circle_position);
	}
	if (vao_circle)
	{
		glDeleteBuffers(1, &vao_circle);
	}
	if (vbo_color_square)
	{
		glDeleteBuffers(1, &vbo_color_square);
	}
	if (vbo_position_square)
	{
		glDeleteBuffers(1, &vbo_position_square);
	}
	if (vao_square)
	{
		glDeleteBuffers(1, &vao_square);
	}
	if (vbo_color_triangle)
	{
		glDeleteBuffers(1, &vbo_color_triangle);
	}
	if (vbo_position_triangle)
	{
		glDeleteBuffers(1, &vbo_position_triangle);
	}
	if (vao_triangle)
	{
		glDeleteBuffers(1, &vao_triangle);
	}
	if (vbo_graph_color)
	{
		glDeleteBuffers(1, &vbo_graph_color);
	}
	if (vbo_graph_position)
	{
		glDeleteBuffers(1, &vbo_graph_position);
	}
	if (vao_graph)
	{
		glDeleteBuffers(1, &vao_graph);
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

