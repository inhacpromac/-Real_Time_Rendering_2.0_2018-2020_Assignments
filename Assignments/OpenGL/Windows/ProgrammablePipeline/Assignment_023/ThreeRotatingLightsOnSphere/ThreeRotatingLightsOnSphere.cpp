// Assignment_023
// Three Rotating Lights On Sphere

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
bool gbAnimation = false;
bool gbLighting = false;
GLuint gNumVertices;
GLuint gNumElements;
GLfloat angleOfLightZero = 0.0f;
GLfloat angleOfLightOne = 0.0f;
GLfloat angleOfLightTwo = 0.0f;
GLfloat speed = 0.1f;


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
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;
GLuint mUniform;
GLuint vUniform;
GLuint pUniform;
GLuint laZeroUniform = 0;
GLuint ldZeroUniform = 0;
GLuint lsZeroUniform = 0;
GLuint laOneUniform = 0;
GLuint ldOneUniform = 0;
GLuint lsOneUniform = 0;
GLuint laTwoUniform = 0;
GLuint ldTwoUniform = 0;
GLuint lsTwoUniform = 0;
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;
GLuint lightZeroPositionUniform = 0;
GLuint lightOnePositionUniform = 0;
GLuint lightTwoPositionUniform = 0;
GLuint lightingEnabledUniform;
mat4 perspectiveProjectionMatrix;

GLfloat lightZeroAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroDiffuse[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroSpecular[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroPosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat lightOneAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightOneDiffuse[] = { 0.0f, 1.0f, 0.0f, 0.0f };
GLfloat lightOneSpecular[] = { 0.0f, 1.0f, 0.0f, 0.0f };
GLfloat lightOnePosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat lightTwoAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightTwoDiffuse[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat lightTwoSpecular[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat lightTwoPosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat materialShininess = 128.0f;	//50.0f

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

// global function declarations
void ToggleFullScreen(void);
void resize(int, int);
void uninitialize();
void update(void);

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
				if (gbAnimation)
				{
					update();
				}
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

	///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
	//Define Vertex Shader Object
	gVertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	//Write Vertex Shader Object
	const GLchar* vertexShaderSourceCode =
		/*"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeypressed;" \
		"uniform vec3 u_la_zero;" \
		"uniform vec3 u_ld_zero;" \
		"uniform vec3 u_ls_zero;" \
		"uniform vec3 u_la_one;" \
		"uniform vec3 u_ld_one;" \
		"uniform vec3 u_ls_one;" \
		"uniform vec3 u_la_two;" \
		"uniform vec3 u_ld_two;" \
		"uniform vec3 u_ls_two;" \
		"uniform vec3 u_ka;" \
		"uniform vec3 u_kd;" \
		"uniform vec3 u_ks;" \
		"uniform vec4 u_light_position_zero;" \
		"uniform vec4 u_light_position_one;" \
		"uniform vec4 u_light_position_two;" \
		"uniform float u_material_shininess;" \
		"out vec3 phong_ads_light;" \
		"void main(void)" \
		"{" \
		"   if(u_lkeypressed == 1)" \
		"   {" \
		"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
		"       vec3 tNorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" \
		"       vec3 light_direction_zero = normalize(vec3(u_light_position_zero - eye_coordinates));" \
		"       vec3 light_direction_one = normalize(vec3(u_light_position_one - eye_coordinates));" \
		"       vec3 light_direction_two = normalize(vec3(u_light_position_two - eye_coordinates));" \
		"       float tn_dot_ld_zero = max(dot(light_direction_zero, tNorm), 0.0);" \
		"       float tn_dot_ld_one = max(dot(light_direction_one, tNorm), 0.0);" \
		"       float tn_dot_ld_two = max(dot(light_direction_two, tNorm), 0.0);" \
		"       vec3 reflection_vector_zero = reflect(-light_direction_zero, tNorm);" \
		"       vec3 reflection_vector_one = reflect(-light_direction_one, tNorm);" \
		"       vec3 reflection_vector_two = reflect(-light_direction_two, tNorm);" \
		"       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
		"       vec3 ambient_zero = u_la_zero * u_ka;" \
		"       vec3 ambient_one = u_la_one * u_ka;" \
		"       vec3 ambient_two = u_la_two * u_ka;" \
		"       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" \
		"       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" \
		"       vec3 diffuse_two = u_ld_one * u_kd * tn_dot_ld_two;" \
		"       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, viewer_vector), 0.0), u_material_shininess);" \
		"       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, viewer_vector), 0.0), u_material_shininess);" \
		"       vec3 specular_two = u_ls_two * u_ks * pow(max(dot(reflection_vector_two, viewer_vector), 0.0), u_material_shininess);" \
		"       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one + ambient_two + diffuse_two + specular_two;"
		"   }" \
		"   else" \
		"   {" \
		"       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"   }" \
		"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
		"}";*/
		"#version 450 core" \
		"\n" \
		"in vec4 vPosition;" \
		"in vec3 vNormal;" \
		"uniform mat4 u_m_matrix;" \
		"uniform mat4 u_v_matrix;" \
		"uniform mat4 u_p_matrix;" \
		"uniform int u_lkeypressed;" \
		"uniform vec4 u_light_position_zero;" \
		"uniform vec4 u_light_position_one;" \
		"uniform vec4 u_light_position_two;" \
		"out vec3 tNorm;" \
		"out vec3 light_direction_zero;" \
		"out vec3 light_direction_one;" \
		"out vec3 light_direction_two;" \
		"out vec3 viewer_vector;" \
		"void main(void)" \
		"{" \
		"   if(u_lkeypressed == 1)" \
		"   {" \
		"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
		"       tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
		"       light_direction_zero = vec3(u_light_position_zero - eye_coordinates);" \
		"       light_direction_one = vec3(u_light_position_one - eye_coordinates);" \
		"       light_direction_two = vec3(u_light_position_two - eye_coordinates);" \
		"       viewer_vector = -eye_coordinates.xyz;" \
		"   }" \
		"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
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
		/*"#version 450 core" \
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
		"}";*/
		"#version 450 core" \
		"\n" \
		"in vec3 tNorm;" \
		"in vec3 light_direction_zero;" \
		"in vec3 light_direction_one;" \
		"in vec3 light_direction_two;" \
		"in vec3 viewer_vector;" \
		"uniform int u_lkeypressed;" \
		"uniform vec3 u_la_zero;" \
		"uniform vec3 u_ld_zero;" \
		"uniform vec3 u_ls_zero;" \
		"uniform vec3 u_la_one;" \
		"uniform vec3 u_ld_one;" \
		"uniform vec3 u_ls_one;" \
		"uniform vec3 u_la_two;" \
		"uniform vec3 u_ld_two;" \
		"uniform vec3 u_ls_two;" \
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
		"       vec3 normalized_viewer_vector = normalize(viewer_vector);" \
		"		\n" \
		"       vec3 normalized_light_direction_zero = normalize(light_direction_zero);" \
		"       float tn_dot_ld_zero = max(dot(normalized_light_direction_zero, normalized_tNorm), 0.0);" \
		"       vec3 ambient_zero = u_la_zero * u_ka;" \
		"       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" \
		"       vec3 reflection_vector_zero = reflect(-normalized_light_direction_zero, normalized_tNorm);" \
		"       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, normalized_light_direction_zero), 0.0), u_material_shininess);" \
		"		\n" \
		"       vec3 normalized_light_direction_one = normalize(light_direction_one);" \
		"       float tn_dot_ld_one = max(dot(normalized_light_direction_one, normalized_tNorm), 0.0);" \
		"       vec3 ambient_one = u_la_one * u_ka;" \
		"       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" \
		"       vec3 reflection_vector_one = reflect(-normalized_light_direction_one, normalized_tNorm);" \
		"       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, normalized_light_direction_one), 0.0), u_material_shininess);" \
		"		\n" \
		"       vec3 normalized_light_direction_two = normalize(light_direction_two);" \
		"       float tn_dot_ld_two = max(dot(normalized_light_direction_two, normalized_tNorm), 0.0);" \
		"       vec3 ambient_two = u_la_two * u_ka;" \
		"       vec3 diffuse_two = u_ld_two * u_kd * tn_dot_ld_two;" \
		"       vec3 reflection_vector_two = reflect(-normalized_light_direction_two, normalized_tNorm);" \
		"       vec3 specular_two = u_ls_two * u_ks * pow(max(dot(reflection_vector_two, normalized_light_direction_two), 0.0), u_material_shininess);" \
		"		\n" \
		"       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one + ambient_two + diffuse_two + specular_two;"
		"   }" \
		"   else" \
		"   {" \
		"       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
		"   }" \
		"	FragColor = vec4(phong_ads_light, 1.0);" \
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
	glBindAttribLocation(gShaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
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
	lightingEnabledUniform = glGetUniformLocation(gShaderProgramObject, "u_lkeypressed");
	mUniform = glGetUniformLocation(gShaderProgramObject, "u_m_matrix");
	vUniform = glGetUniformLocation(gShaderProgramObject, "u_v_matrix");
	pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");
	laZeroUniform = glGetUniformLocation(gShaderProgramObject, "u_la_zero");
	ldZeroUniform = glGetUniformLocation(gShaderProgramObject, "u_ld_zero");
	lsZeroUniform = glGetUniformLocation(gShaderProgramObject, "u_ls_zero");
	laOneUniform = glGetUniformLocation(gShaderProgramObject, "u_la_one");
	ldOneUniform = glGetUniformLocation(gShaderProgramObject, "u_ld_one");
	lsOneUniform = glGetUniformLocation(gShaderProgramObject, "u_ls_one");
	laTwoUniform = glGetUniformLocation(gShaderProgramObject, "u_la_two");
	ldTwoUniform = glGetUniformLocation(gShaderProgramObject, "u_ld_two");
	lsTwoUniform = glGetUniformLocation(gShaderProgramObject, "u_ls_two");
	kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
	kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
	ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
	materialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
	lightZeroPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position_zero");
	lightOnePositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position_one");
	lightTwoPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position_two");
	///////////////////////////////////////////////////////////////////////////////////////////////

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

	glUseProgram(gShaderProgramObject);

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
		glUniform1i(lightingEnabledUniform, 1);

		glUniform3fv(laZeroUniform, 1, lightZeroAmbient);
		glUniform3fv(ldZeroUniform, 1, lightZeroDiffuse);
		glUniform3fv(lsZeroUniform, 1, lightZeroSpecular);

		glUniform3fv(laOneUniform, 1, lightOneAmbient);
		glUniform3fv(ldOneUniform, 1, lightOneDiffuse);
		glUniform3fv(lsOneUniform, 1, lightOneSpecular);

		glUniform3fv(laTwoUniform, 1, lightTwoAmbient);
		glUniform3fv(ldTwoUniform, 1, lightTwoDiffuse);
		glUniform3fv(lsTwoUniform, 1, lightTwoSpecular);

		glUniform3fv(kaUniform, 1, materialAmbient);
		glUniform3fv(kdUniform, 1, materialDiffuse);
		glUniform3fv(ksUniform, 1, materialSpecular);

		glUniform1f(materialShininessUniform, materialShininess);

		glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition);
		glUniform4fv(lightOnePositionUniform, 1, lightOnePosition);
		glUniform4fv(lightTwoPositionUniform, 1, lightTwoPosition);
	}
	else
	{
		glUniform1i(lightingEnabledUniform, 0);
	}

	//Send necessary Matrix to Shader in respective Uniform
	glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(vUniform, 1, GL_FALSE, ViewMatrix);
	glUniformMatrix4fv(pUniform, 1, GL_FALSE, projectionMatrix);

	//Bind with vao
	glBindVertexArray(gVao_sphere);

	//simillar Bind with Texture if any


	//Draw necessary Scence
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//Unbind with vao
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);
}

void update(void)
{
	angleOfLightZero += speed;
	angleOfLightOne += speed;
	angleOfLightTwo += speed;

	if (angleOfLightZero >= 360.0f)
	{
		angleOfLightZero = 0.0f;
	}
	lightZeroPosition[0] = 0.0f;
	lightZeroPosition[1] = sinf(angleOfLightZero) * 100.0f;
	lightZeroPosition[2] = cosf(angleOfLightZero) * 100.0f - 5.0f;

	if (angleOfLightOne >= 360.0f)
	{
		angleOfLightOne = 0.0f;
	}
	lightOnePosition[0] = sinf(angleOfLightOne) * 100.0f;
	lightOnePosition[1] = 0.0f;
	lightOnePosition[2] = cosf(angleOfLightOne) * 100.0f - 5.0f;

	if (angleOfLightTwo >= 360.0f)
	{
		angleOfLightTwo = 0.0f;
	}
	lightTwoPosition[0] = cosf(angleOfLightTwo) * 100.0f;
	lightTwoPosition[1] = sinf(angleOfLightTwo) * 100.0f;
	lightTwoPosition[2] = -5.0f;
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

