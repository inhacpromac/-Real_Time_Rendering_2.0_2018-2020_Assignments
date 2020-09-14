// Assignment_021
// perVertex perFragment Toggle

// header
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<glew.h>
#include<gl/GL.h>
#define _USE_MATH_DEFINES 1
#include <math.h>
#include "vmath.h"
#include "Sphere.h"

using namespace vmath;

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"Sphere.lib")

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
//bool gbAnimation = false;
bool gbLighting = false;
bool isPerVertexLightEnabled = false;
GLuint gNumVertices;
GLuint gNumElements;

GLuint gShaderProgramObjectPerVertex;
GLuint gShaderProgramObjectPerFragment;
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_NORMAL,
	AMC_ATTRIBUTE_TEXCOORD0,
};
//vao-shapes
//vbo-color,texture,....
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint mUniformPerVertex;
GLuint vUniformPerVertex;
GLuint pUniformPerVertex;

GLuint laUniformPerVertex = 0;
GLuint ldUniformPerVertex = 0;
GLuint lsUniformPerVertex = 0;
GLuint kaUniformPerVertex = 0;
GLuint kdUniformPerVertex = 0;
GLuint ksUniformPerVertex = 0;
GLuint materialShininessUniformPerVertex = 0;
GLuint lightPositionUniformPerVertex = 0;
GLuint lightingEnabledUniformPerVertex;

GLuint mUniformPerFragment;
GLuint vUniformPerFragment;
GLuint pUniformPerFragment;
GLuint laUniformPerFragment = 0;
GLuint ldUniformPerFragment = 0;
GLuint lsUniformPerFragment = 0;
GLuint kaUniformPerFragment = 0;
GLuint kdUniformPerFragment = 0;
GLuint ksUniformPerFragment = 0;
GLuint materialShininessUniformPerFragment = 0;
GLuint lightPositionUniformPerFragment = 0;
GLuint lightingEnabledUniformPerFragment;
mat4 perspectiveProjectionMatrix;

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 128.0f;	//50.0f

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

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
		case 'q':
		case 'Q':
			DestroyWindow(hwnd);
			break;
		case VK_ESCAPE:
			ToggleFullScreen();
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
		case 'v':
		case 'V':
			isPerVertexLightEnabled = true;
			break;
		case 'f':
		case 'F':
			isPerVertexLightEnabled = false;
			break;
		/*case 'a':
		case 'A':
			if (gbAnimation == false)
			{
				gbAnimation = true;
			}
			else if (gbAnimation == true)
			{
				gbAnimation = false;
			}
			break;*/
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

	GLuint gVertexShaderObjectPerVertex;
	GLuint gFragmentShaderObjectPerVertex;

	GLuint gVertexShaderObjectPerFragment;
	GLuint gFragmentShaderObjectPerFragment;

	GLenum result;
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

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

//xx//xx//xx//PerVertexInitilization//xx//xx//xx//

//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
	///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
	//Define Vertex Shader Object
	gVertexShaderObjectPerVertex = glCreateShader(GL_VERTEX_SHADER);
	//Write Vertex Shader Object
	const GLchar* vertexShaderSourceCodePerVertex =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeypressed;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform vec4 u_light_position;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"   if(u_lkeypressed == 1)" \
		"   {" \
		"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
		"       vec3 tNorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" \
		"       vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));" \
		"       float tn_dot_ld = max(dot(light_direction, tNorm), 0.0);" \
		"       vec3 reflection_vector = reflect(-light_direction, tNorm);" \
		"       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
		"       vec3 ambient = u_la * u_ka;" \
		"       vec3 diffuse = u_ld * u_kd * tn_dot_ld;" \
		"       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
		"       phong_ads_light = ambient + diffuse + specular;"
		"   }" \
		"   else" \
		"   {" \
		"       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"   }" \
		"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
		"}";

	//Specifying Above code to the VertexShaderObject
	glShaderSource(gVertexShaderObjectPerVertex, 1, (const char**)&vertexShaderSourceCodePerVertex, NULL);
	//Compile the Vertex Shader
	glCompileShader(gVertexShaderObjectPerVertex);
	//Error Checking for Vertex Shader Compiling
	GLint iShaderCompileStatus = 0;
	GLint iInfoLogLength = 0;
	GLchar* szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObjectPerVertex, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectPerVertex, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Vertex Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
	//Define Fragment Shader Object
	gFragmentShaderObjectPerVertex = glCreateShader(GL_FRAGMENT_SHADER);
	//Write Fragment Shader Object
	const GLchar* fragmentShaderSourceCodePerVertex =
		"#version 450 core" \
		"\n" \
		"in vec3 phong_ads_light;" \
		"out vec4 FragColor;" \
		"uniform int u_lkeypressed;" \
		"void main(void)" \
		"{" \
		"   if(u_lkeypressed == 1)" \
		"   {" \
		"       FragColor = vec4(phong_ads_light, 1.0);" \
		"   }" \
		"   else" \
		"   {" \
		"       FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
		"   }" \
		"}";

	//Specifying Above code to the FragmentShaderObject
	glShaderSource(gFragmentShaderObjectPerVertex, 1, (const char**)&fragmentShaderSourceCodePerVertex, NULL);
	//Compile the fragment Shader
	glCompileShader(gFragmentShaderObjectPerVertex);
	//Error Checking for Fragment Shader Compiling
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObjectPerVertex, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectPerVertex, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Fragment Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
	//Create Shader Program Object
	gShaderProgramObjectPerVertex = glCreateProgram();
	//Attach Vertex Shader to Shader Program
	glAttachShader(gShaderProgramObjectPerVertex, gVertexShaderObjectPerVertex);
	//Attach Fragment Shader to Shader Program
	glAttachShader(gShaderProgramObjectPerVertex, gFragmentShaderObjectPerVertex);
	//preLinking Binding to Vertex Attributes
	glBindAttribLocation(gShaderProgramObjectPerVertex, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerVertex, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Link The Shader Program
	glLinkProgram(gShaderProgramObjectPerVertex);
	//Error Checking for Shader Program Linking
	GLint iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObjectPerVertex, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerVertex, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Shader Program Link Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//postLinking retriving uniform location
	lightingEnabledUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_lkeypressed");
	mUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_m_matrix");
	vUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_v_matrix");
	pUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_p_matrix");
	laUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_la");
	ldUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ld");
	lsUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ls");
	kaUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ka");
	kdUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_kd");
	ksUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ks");
	materialShininessUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_material_shininess");
	lightPositionUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_light_position");
	///////////////////////////////////////////////////////////////////////////////////////////////
//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//



//xx//xx//xx//PerFragmentInitilization//xx//xx//xx//
			  
//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
	///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
	//Define Vertex Shader Object
	gVertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER);
	//Write Vertex Shader Object
	const GLchar* vertexShaderSourceCodePerFragment =
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeypressed;" \
		"uniform vec4 u_light_position;" \
		"out vec3 tNorm;" \
		"out vec3 light_direction;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"   if(u_lkeypressed == 1)" \
		"   {" \
		"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
		"       tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
		"       light_direction = vec3(u_light_position - eye_coordinates);" \
		"       viewer_vector = -eye_coordinates.xyz;" \
		"   }" \
		"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
		"}";

	//Specifying Above code to the VertexShaderObject
	glShaderSource(gVertexShaderObjectPerFragment, 1, (const char**)&vertexShaderSourceCodePerFragment, NULL);
	//Compile the Vertex Shader
	glCompileShader(gVertexShaderObjectPerFragment);
	//Error Checking for Vertex Shader Compiling
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectPerFragment, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Vertex Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
	//Define Fragment Shader Object
	gFragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER);
	//Write Fragment Shader Object
	const GLchar* fragmentShaderSourceCodePerFragment =
		"#version 450 core" \
		"\n" \
		"in vec3 tNorm;" \
		"in vec3 light_direction;" \
		"in vec3 viewer_vector;" \
		"uniform int u_lkeypressed;" \
		"uniform vec3 u_la;" \
		"uniform vec3 u_ld;" \
		"uniform vec3 u_ls;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform float u_material_shininess;" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"   vec3 phong_ads_light;" \
		"   if(u_lkeypressed == 1)" \
		"   {" \
		"       vec3 normalized_tNorm = normalize(tNorm);" \
		"       vec3 normalized_light_direction = normalize(light_direction);" \
		"       vec3 normalized_viewer_vector = normalize(viewer_vector);" \
		"       float tn_dot_ld = max(dot(normalized_light_direction, normalized_tNorm), 0.0);" \
		"       vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tNorm);" \
		"       vec3 ambient = u_la * u_ka;" \
		"       vec3 diffuse = u_ld * u_kd * tn_dot_ld;" \
		"       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" \
		"       phong_ads_light = ambient + diffuse + specular;"
		"   }" \
		"   else" \
		"   {" \
		"       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"   }" \
		"	FragColor = vec4(phong_ads_light, 1.0);" \
		"}";

	//Specifying Above code to the FragmentShaderObject
	glShaderSource(gFragmentShaderObjectPerFragment, 1, (const char**)&fragmentShaderSourceCodePerFragment, NULL);
	//Compile the fragment Shader
	glCompileShader(gFragmentShaderObjectPerFragment);
	//Error Checking for Fragment Shader Compiling
	iShaderCompileStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetShaderiv(gFragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompileStatus);
	if (iShaderCompileStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectPerFragment, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Fragment Shader Compile Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
	//Create Shader Program Object
	gShaderProgramObjectPerFragment = glCreateProgram();
	//Attach Vertex Shader to Shader Program
	glAttachShader(gShaderProgramObjectPerFragment, gVertexShaderObjectPerFragment);
	//Attach Fragment Shader to Shader Program
	glAttachShader(gShaderProgramObjectPerFragment, gFragmentShaderObjectPerFragment);
	//preLinking Binding to Vertex Attributes
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_POSITION, "vPosition");
	glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_NORMAL, "vNormal");
	//Link The Shader Program
	glLinkProgram(gShaderProgramObjectPerFragment);
	//Error Checking for Shader Program Linking
	iProgramLinkStatus = 0;
	iInfoLogLength = 0;
	szInfoLog = NULL;
	glGetProgramiv(gShaderProgramObjectPerFragment, GL_LINK_STATUS, &iProgramLinkStatus);
	if (iProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(gShaderProgramObjectPerFragment, iInfoLogLength, &written, szInfoLog);
				fprintf(gpfile, "Shader Program Link Log:\n%s\n", szInfoLog);
				free(szInfoLog);
				uninitialize();
				exit(0);
			}
		}
	}
	//postLinking retriving uniform location
	lightingEnabledUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_lkeypressed");
	mUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_m_matrix");
	vUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_v_matrix");
	pUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_p_matrix");
	laUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_la");
	ldUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ld");
	lsUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ls");
	kaUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ka");
	kdUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_kd");
	ksUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ks");
	materialShininessUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_material_shininess");
	lightPositionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_light_position");
	///////////////////////////////////////////////////////////////////////////////////////////////
//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//


	// vao
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// position vbo
	glGenBuffers(1, &gVbo_sphere_position);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	perspectiveProjectionMatrix = mat4::identity();

	glDisable(GL_CULL_FACE);

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
	perspectiveProjectionMatrix = perspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (isPerVertexLightEnabled)
	{
		glUseProgram(gShaderProgramObjectPerVertex);

		//Declaration of Matrices
		mat4 modelMatrix;
		mat4 ViewMatrix;
		mat4 projectionMatrix;
		mat4 translationMatrix;

		//Initialize above changed Matrices to identity
		modelMatrix = mat4::identity();
		ViewMatrix = mat4::identity();
		projectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();

		//Do necessary Trasformation
		translationMatrix = translate(0.0f, 0.0f, -3.0f);

		//Do necessary Matrix Multiplication
		modelMatrix = modelMatrix * translationMatrix;
		projectionMatrix = projectionMatrix * perspectiveProjectionMatrix;

		if (gbLighting == true)
		{
			glUniform1i(lightingEnabledUniformPerVertex, 1);
			glUniform3fv(laUniformPerVertex, 1, lightAmbient);
			glUniform3fv(ldUniformPerVertex, 1, lightDiffuse);
			glUniform3fv(lsUniformPerVertex, 1, lightSpecular);
			glUniform3fv(kaUniformPerVertex, 1, materialAmbient);
			glUniform3fv(kdUniformPerVertex, 1, materialDiffuse);
			glUniform3fv(ksUniformPerVertex, 1, materialSpecular);
			glUniform1f(materialShininessUniformPerVertex, materialShininess);
			glUniform4fv(lightPositionUniformPerVertex, 1, lightPosition);
		}
		else
		{
			glUniform1i(lightingEnabledUniformPerVertex, 0);
		}

		//Send necessary Matrix to Shader in respective Uniform
		glUniformMatrix4fv(mUniformPerVertex, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(vUniformPerVertex, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(pUniformPerVertex, 1, GL_FALSE, projectionMatrix);

		//Bind with vao
		glBindVertexArray(gVao_sphere);

		//simillar Bind with Texture if any


		//Draw necessary Scence
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

		//Unbind with vao
		glBindVertexArray(0);

		glUseProgram(0);
	}
	else
	{
		glUseProgram(gShaderProgramObjectPerFragment);

		//Declaration of Matrices
		mat4 modelMatrix;
		mat4 ViewMatrix;
		mat4 projectionMatrix;
		mat4 translationMatrix;

		//Initialize above changed Matrices to identity
		modelMatrix = mat4::identity();
		ViewMatrix = mat4::identity();
		projectionMatrix = mat4::identity();
		translationMatrix = mat4::identity();

		//Do necessary Trasformation
		translationMatrix = translate(0.0f, 0.0f, -3.0f);

		//Do necessary Matrix Multiplication
		modelMatrix = modelMatrix * translationMatrix;
		projectionMatrix = projectionMatrix * perspectiveProjectionMatrix;

		if (gbLighting == true)
		{
			glUniform1i(lightingEnabledUniformPerFragment, 1);
			glUniform3fv(laUniformPerFragment, 1, lightAmbient);
			glUniform3fv(ldUniformPerFragment, 1, lightDiffuse);
			glUniform3fv(lsUniformPerFragment, 1, lightSpecular);
			glUniform3fv(kaUniformPerFragment, 1, materialAmbient);
			glUniform3fv(kdUniformPerFragment, 1, materialDiffuse);
			glUniform3fv(ksUniformPerFragment, 1, materialSpecular);
			glUniform1f(materialShininessUniformPerFragment, materialShininess);
			glUniform4fv(lightPositionUniformPerFragment, 1, lightPosition);
		}
		else
		{
			glUniform1i(lightingEnabledUniformPerFragment, 0);
		}

		//Send necessary Matrix to Shader in respective Uniform
		glUniformMatrix4fv(mUniformPerFragment, 1, GL_FALSE, modelMatrix);
		glUniformMatrix4fv(vUniformPerFragment, 1, GL_FALSE, ViewMatrix);
		glUniformMatrix4fv(pUniformPerFragment, 1, GL_FALSE, projectionMatrix);

		//Bind with vao
		glBindVertexArray(gVao_sphere);

		//simillar Bind with Texture if any


		//Draw necessary Scence
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
		glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

		//Unbind with vao
		glBindVertexArray(0);

		glUseProgram(0);
	}
	

	SwapBuffers(ghdc);
}

void uninitialize(void)
{
	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
	}
	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
	}
	if (gVbo_sphere_position)
	{
		glDeleteBuffers(1, &gVbo_sphere_position);
	}
	if (gVao_sphere)
	{
		glDeleteBuffers(1, &gVao_sphere);
	}
	if (gShaderProgramObjectPerFragment)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObjectPerFragment);

		//Ask Program: How many Programs are attched to you?
		glGetProgramiv(gShaderProgramObjectPerFragment, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders = (GLuint*)malloc(2 * sizeof(shaderCount));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerFragment, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//Detach everyone
				glDetachShader(gShaderProgramObjectPerFragment, pShaders[shaderNumber]);

				//Delete The Detached Shaders
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObjectPerFragment);
		gShaderProgramObjectPerFragment = 0;
		glUseProgram(0);
	}
	
	if (gShaderProgramObjectPerVertex)
	{
		GLsizei shaderCount;
		GLsizei shaderNumber;
		glUseProgram(gShaderProgramObjectPerVertex);

		//Ask Program: How many Programs are attched to you?
		glGetProgramiv(gShaderProgramObjectPerVertex, GL_ATTACHED_SHADERS, &shaderCount);
		GLuint* pShaders = (GLuint*)malloc(2 * sizeof(shaderCount));
		if (pShaders)
		{
			glGetAttachedShaders(gShaderProgramObjectPerVertex, shaderCount, &shaderCount, pShaders);
			for (shaderNumber = 0; shaderNumber < shaderCount; shaderNumber++)
			{
				//Detach everyone
				glDetachShader(gShaderProgramObjectPerVertex, pShaders[shaderNumber]);

				//Delete The Detached Shaders
				pShaders[shaderNumber] = 0;
			}
			free(pShaders);
		}
		glDeleteProgram(gShaderProgramObjectPerVertex);
		gShaderProgramObjectPerVertex = 0;
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

