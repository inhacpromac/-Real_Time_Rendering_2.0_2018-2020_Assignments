//Header Files
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <gl/glew.h> // IMPORTANT : THIS FILE MUST BE INCLUDED BEFORE <gl/GL.h >  & <gl/GLU.h>
#include <gl/GL.h>
#include <math.h>
#include <vector>
#include "uf/vmath.h"
#include "uf/utilheader.h"
#include "uf/loadshader.h"
#include "uf/objloader.h"
#include "uf/resource.h"


//Linking with Opengl Library (This is for linker)
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

//Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600


// global Variable declarations
bool gbFullscreen 		= false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev 	= { sizeof(WINDOWPLACEMENT) };
HWND ghwnd 				= NULL;
HDC ghdc 				= NULL;
HGLRC ghrc 				= NULL;
bool gbActiveWindow 	= false;
bool gbAnimation 		= false;
bool gbLighting 		= false;


GLenum result;

GLuint vs;
GLuint fs;

//for teapot
GLuint program;

GLuint mUniform;
GLuint vUniform;
GLuint pUniform;
GLuint samplerUniform1;
GLuint Texture_Marble;
GLuint laUniform = 0;
GLuint ldUniform = 0;
GLuint lsUniform = 0;
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;
GLuint lightPositionUniform = 0;
GLuint lightingEnabledUniform;
mat4 PerspectiveProjectionMatrix;

GLfloat lightAmbient[] = { 0.25f, 0.25f, 0.25f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 128.0f;	//50.0f

GLfloat rAngle;

//for cube
GLuint program2;
GLuint vao_cube;
GLuint vbo_position_cube;
GLuint vbo_texture_cube;
GLuint mvpUniform;
GLuint samplerUniform2;

GLuint glWidth;
GLuint glHeight;

//for framebuffer
GLuint fbo;
GLuint color_texture;
GLuint depth_texture;

//Function declaration
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//Main Function - WinMain()
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function declarations
	int initialize(void);
	void display(void);
	void update(void);

	// variable declarations
	WNDCLASSEX wndClass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");
	bool bDone = false;
	int iRet = 0;

	if (fopen_s(&gpFile, "Log.txt", "w") != 0)
	{
		MessageBox(NULL, 
			TEXT("Log File Can not be Created"), 
			TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Log file is Sucessfully created \n");
	}

	// code
	// initialization of WNDCLASSEX
	wndClass.cbSize 		= sizeof(WNDCLASSEX);
	wndClass.style 			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.cbClsExtra 	= 0;
	wndClass.cbWndExtra 	= 0;
	wndClass.lpfnWndProc 	= WndProc;
	wndClass.hInstance 		= hInstance;
	wndClass.hIcon 			= LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor 		= LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground 	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszClassName 	= szAppName;
	wndClass.lpszMenuName 	= NULL;
	wndClass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);

	// register above class
	RegisterClassEx(&wndClass);

	// get mid point dimensions
	int screenX = ((GetSystemMetrics(SM_CXSCREEN)/2) - (WIN_WIDTH  / 2));
	int screenY = ((GetSystemMetrics(SM_CYSCREEN)/2) - (WIN_HEIGHT / 2));

	// create windowex
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Render To Texture - INHACPROMAC"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		screenX,
		screenY,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	iRet = initialize(); // here we should check the value of iRet for errors

	if (iRet == -1)
	{
		fprintf(gpFile, "ChoosePixelFormat() failed\n");
		DestroyWindow(hwnd);
		exit(0);
	}
	else if (iRet == -2)
	{
		fprintf(gpFile, "SetPixelFormat() failed\n");
		DestroyWindow(hwnd);
		exit(0);
	}
	else if (iRet == -3)
	{
		fprintf(gpFile, "wglCreateContext() failed\n");
		DestroyWindow(hwnd);
		exit(0);
	}
	else if (iRet == -4)
	{
		fprintf(gpFile, "wglMakeCurrentContext() failed\n");
		DestroyWindow(hwnd);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Initialization Sucessfull !n");
	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	//Here instead of calling UpdateWindow(), call below 2 new functions
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game loop
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
			//Game Loop - Here the rendering happens
			if (gbActiveWindow == true)
			{
				// Here call update()

			}
			// render function
			display();
		}
	}

	return((int)msg.wParam);
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//Function declarations
	void ToggleFullscreen(void);
	void resize(int, int);
	void uninitialize(void);

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
		glWidth = LOWORD(lParam);
		glHeight = HIWORD(lParam);
		break;

	case WM_ERASEBKGND:
		return (0);

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
			case VK_ESCAPE:
				DestroyWindow(hwnd);
				break;
			//case 0x46:
			case 0x46:
				ToggleFullscreen();
				break;
			case 'l':
			case 'L':
				if (gbLighting == false)
				{
					gbLighting = true;
				}
				else if (gbLighting == true)
				{
					gbLighting = false;
				}
				break;
			case 'a':
			case 'A':
				if (gbAnimation == false)
				{
					gbAnimation = true;
				}
				else if (gbAnimation == true)
				{
					gbAnimation = false;
				}
				break;
			default:
				break;
		}
		break;

	case WM_DESTROY:
		uninitialize();
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	//code
	MONITORINFO mi;
	BOOL loadTexture(GLuint*, TCHAR[]);

	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if (GetWindowPlacement(ghwnd, &wpPrev)
				&& GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

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
		gbFullscreen = true;
	}
	else
	{
		SetWindowLong(ghwnd,
			GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
		gbFullscreen = false;
	}
}

int initialize(void)
{
	//function declarations
	void resize(int, int);
	void uninitialize(void);
	BOOL loadTexture(GLuint*, TCHAR[]);

	// variable declarations
	static PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex;

	// code
	// Initialize pfd structure
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize 		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion 	= 1;
	pfd.dwFlags 	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType 	= PFD_TYPE_RGBA;
	pfd.cColorBits 	= 32;
	pfd.cRedBits 	= 8;
	pfd.cGreenBits 	= 8;
	pfd.cBlueBits 	= 8;
	pfd.cAlphaBits 	= 8;
	pfd.cDepthBits 	= 24;

	ghdc = GetDC(ghwnd);

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	//return index is always 1 based means it zero get return is failure
	if (iPixelFormatIndex == 0)
	{
		return (-1);
	}

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		return (-2);
	}

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		return(-3);
	}
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		return(-4);
	}

	//GLEW intialization code for GLSL
	// IMPORTANT : it must be here.means after creating OpenGL Context
	// before using any opengl function
	result = glewInit();
	if (result != GLEW_OK)
	{
		fprintf(gpFile, "glewInit error.Exiting now\n");
		uninitialize();
	}

	//// SHADER PROGRAM 1 ////
	GLint iProgramLinkStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar *szInfoLog = NULL;

	// *** VERTEX SHADER ***
	vs = LoadShaders("./MEDIA/shaders/vertexshader.vs", GL_VERTEX_SHADER);
	if (!vs)
	{
		uninitialize();
		exit(0);
	}

	// *** FRAGMENT SHADER ***
	fs = LoadShaders("./MEDIA/shaders/fragmentshader.fs", GL_FRAGMENT_SHADER);
	if (!fs)
	{
		uninitialize();
		exit(0);
	}

	// *** SHADER PROGRAM ***
	// create 
	program = glCreateProgram();

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	// prelinking binding to vertex attribute //
	glBindAttribLocation(program, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(program, AMC_ATTRIBUTE_NORMALS, "vNormal");
	glBindAttribLocation(program, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");

	// Link the shader program
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(program, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program 1 Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(NULL);
				exit(0);
			}
		}
	}

	// postlinking receiving uniform location //
	lightingEnabledUniform = glGetUniformLocation(program, "u_lkeypressed");
	mUniform = glGetUniformLocation(program, "u_m_matrix");
	vUniform = glGetUniformLocation(program, "u_v_matrix");
	pUniform = glGetUniformLocation(program, "u_p_matrix");
	laUniform = glGetUniformLocation(program, "u_la");
	ldUniform = glGetUniformLocation(program, "u_ld");
	lsUniform = glGetUniformLocation(program, "u_ls");
	kaUniform = glGetUniformLocation(program, "u_ka");
	kdUniform = glGetUniformLocation(program, "u_kd");
	ksUniform = glGetUniformLocation(program, "u_ks");
	materialShininessUniform = glGetUniformLocation(program, "u_material_shininess");
	lightPositionUniform = glGetUniformLocation(program, "u_light_position");
	samplerUniform1 = glGetUniformLocation(program, "u_sampler");

	//// SHADER PROGRAM 2 ////
	//reinitialize
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;

	// *** VERTEX SHADER ***
	vs = LoadShaders("./MEDIA/shaders/basicfbo_vs.vs", GL_VERTEX_SHADER);
	if (!vs)
	{
		uninitialize();
		exit(0);
	}

	// *** FRAGMENT SHADER ***
	fs = LoadShaders("./MEDIA/shaders/basicfbo_fs.fs", GL_FRAGMENT_SHADER);
	if (!fs)
	{
		uninitialize();
		exit(0);
	}

	// *** SHADER PROGRAM ***
	// create 
	program2 = glCreateProgram();

	glAttachShader(program2, vs);
	glAttachShader(program2, fs);

	// prelinking binding to vertex attribute //
	glBindAttribLocation(program2, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(program2, AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");


	// Link the shader program2
	glLinkProgram(program2);
	glGetProgramiv(program2, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(program2, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(program2, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Shader Program 2 Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				DestroyWindow(NULL);
				exit(0);
			}
		}
	}

	glDetachShader(program2, vs);
	glDeleteShader(vs);
	vs = 0;
	glDetachShader(program2, fs);
	glDeleteShader(fs);
	fs = 0;

	// postlinking receiving uniform location //
	mvpUniform	 			= glGetUniformLocation(program2, "u_mvp_matrix");
	samplerUniform2 		= glGetUniformLocation(program2, "u_sampler");

	const GLfloat cubeVertices[] = {
		// Top face
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		// Bottom face
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// Front face
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		// Back face
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		// Right face
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		// Left face
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f
	};

	const GLfloat cubeTexCoord[] = {
		// Top face
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		// Bottom face
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		// Front face
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		// Back face
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		// right face
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		// left face
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
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
	
	//texture	
	glEnable(GL_TEXTURE_2D);
	loadTexture(&Texture_Marble, MAKEINTRESOURCE(IDBITMAP_MARBLE));

	// Load Obj Model And Prepare To Draw
	LoadObj("./MEDIA/objects/utahteapot.obj");
	//LoadObj("./MEDIA/objects/examplecube.obj");
	//LoadObj("./MEDIA/objects/spaceplane.obj");

	// clear the screen by opengl color
	glClearColor(0.25f, 0.25f, 0.25f, 0.0f);

	//depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//FrameBufferObjects
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	
	glGenTextures(1, &color_texture);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 512, 512);// only mem 
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, 512, 512);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, color_texture, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	PerspectiveProjectionMatrix = mat4::identity();

	// intialize the values
	rAngle = 0.0f;

	//warm up call to resize
	resize(WIN_WIDTH, WIN_HEIGHT); //function calls

	return(0);
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

void resize(int width, int height)
{
	// code
	if (height == 0)
	{
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	PerspectiveProjectionMatrix = perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

}

void display(void)
{
	//code
	// declaration of matrices //
	mat4 modelMatrix;
	mat4 viewMatrix;
	mat4 projectionMatrix;
	mat4 translationMatrix;
	mat4 rotationMatrix;

	mat4 modelViewProjectionMatrix;

	// initialization above matrices identity //
	modelMatrix 				= mat4::identity();
	viewMatrix 					= mat4::identity();
	projectionMatrix 			= mat4::identity();
	translationMatrix 			= mat4::identity();
	rotationMatrix				= mat4::identity();

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glViewport(0, 0, 512, 512);
	glClearColor(0.5f, 0.5f,0.5f,1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(program);
	
	translationMatrix 			= translate(0.0f, -8.0f, -50.0f);
	modelMatrix 				= translationMatrix;
	rotationMatrix				= rotate(-90.0f, 1.0f, 0.0f, 0.0f);
	//modelMatrix					= modelMatrix * rotationMatrix;
	rotationMatrix				*= rotate(rAngle, 1.0f, 0.0f, 0.0f);
	rotationMatrix				*= rotate(rAngle, 0.0f, 1.0f, 0.0f);
	rotationMatrix				*= rotate(rAngle, 0.0f, 0.0f, 1.0f);
	modelMatrix					= modelMatrix * rotationMatrix;
	projectionMatrix 			= PerspectiveProjectionMatrix * projectionMatrix;

	if (gbLighting == true)
	{
		glUniform1i(lightingEnabledUniform, 1);
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(ksUniform, 1, materialSpecular);
		glUniform1f(materialShininessUniform, materialShininess);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(vUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, projectionMatrix);

	//Work with texture [A][B][U]
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture_Marble);
	glUniform1i(samplerUniform1, 0);

	RenderObject();

	glUseProgram(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(0.0f, 0.0f,0.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, glWidth, glHeight);
	glUseProgram(program2);

	// initialization above matrices identity //
	modelViewProjectionMatrix 	= mat4::identity();
	translationMatrix 			= mat4::identity();
	rotationMatrix				= mat4::identity();

	translationMatrix 			= translate(0.0f, 0.0f, -5.0f);
	modelViewProjectionMatrix 	= translationMatrix;
	rotationMatrix				= mat4::identity();
	rotationMatrix				= rotate(rAngle, 1.0f, 0.0f, 0.0f);
	rotationMatrix				*= rotate(rAngle, 0.0f, 1.0f, 0.0f);
	rotationMatrix				*= rotate(rAngle, 0.0f, 0.0f, 1.0f);
	modelViewProjectionMatrix	= modelViewProjectionMatrix * rotationMatrix;
	modelViewProjectionMatrix 	= PerspectiveProjectionMatrix * modelViewProjectionMatrix;

	glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

	//Bind with vao_cube
	glBindVertexArray(vao_cube);

	//Work with texture [A][B][U]
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, color_texture);
	glUniform1i(samplerUniform2, 0);

	//Draw necessary Scence
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	//Unbind with vao_cube
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);

	if (gbAnimation ==  true)
	{
		rAngle += 0.05f;
		if (rAngle >= 360.0f)
			rAngle = 0.0f;
	}
	
	
}

void uninitialize(void)
{
	//code
	//check whether fullscreen or not if it is, then restore to
	// normal size & then proceed for uninitialization
	if (gbFullscreen == true)
	{
		// code
		SetWindowLong(ghwnd,
			GWL_STYLE,
			dwStyle | WS_OVERLAPPEDWINDOW);

		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);


		ShowCursor(TRUE);
	}

	//break the current context
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

	if (gpFile)
	{
		fprintf(gpFile, "Log file closed Succesfully");
		fclose(gpFile);
		gpFile = NULL;
	}

	glDeleteTextures(1, &Texture_Marble);
	Texture_Marble = 0;

	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	if (vbo_element)
	{
		glDeleteBuffers(1, &vbo_element);
		vbo_element = 0;
	}

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	// safe Release //
	if (program)
	{
		GLsizei ShaderCount;
		GLsizei ShaderNumber;

		glUseProgram(program);

		// ask program how many shaders are attached to you
		glGetProgramiv(program, GL_ATTACHED_SHADERS, &ShaderCount);
		GLuint *pShader = (GLuint *)malloc(sizeof(GLuint) * ShaderCount);
		if (pShader)
		{
			// give me attached shaders
			glGetAttachedShaders(program, ShaderCount, &ShaderCount, pShader);
			for (ShaderNumber = 0; ShaderNumber <= ShaderCount; ShaderNumber++)
			{
				// DetachShader //
				glDetachShader(program, pShader[ShaderNumber]);
				glDeleteShader(pShader[ShaderNumber]);
				pShader[ShaderNumber] = 0;
			}// end for loop

			free(pShader);
		} // end of if block of pShader

		glDeleteProgram(program);
		program = 0;
		glUseProgram(0);
	} // end if block of program//

} // END OF UNINITIALIZE

