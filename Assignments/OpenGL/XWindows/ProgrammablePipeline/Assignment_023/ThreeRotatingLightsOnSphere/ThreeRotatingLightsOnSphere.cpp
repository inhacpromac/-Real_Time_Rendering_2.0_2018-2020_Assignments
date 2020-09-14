// Assignment_023
// Three Rotating Lights On Sphere

//header
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "vmath.h"
#include "Sphere.h"

//namespaces
using namespace std;
using namespace vmath;

//global variable declarations
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int*);
glXCreateContextAttribsARBProc glXCreateContextAttribsARB = NULL;
GLXFBConfig gGLXFBConfig;
GLXContext gGLXContext;

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
GLfloat materialShininess = 128.0f; //50.0f

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

void resize(int, int);
void update(void);

//entry point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	void initialize(void);
	void resize(int, int);
	void display(void);

    //variable declarations
    int winWidth = giWindowWidth;
	int winHeight = giWindowHeight;
	char keys[26];
	bool bDone = false;

	XEvent event;
	KeySym keysym;
	
	//code
    CreateWindow();

    initialize();
	
	//Game Loop
    while(bDone == false)
    {
        while(XPending(gpDisplay))
        {
        	XNextEvent(gpDisplay, &event);
			switch(event.type)
			{
				case MapNotify:
					break;
				case KeyPress:
					keysym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0);
					switch(keysym)
					{
						case XK_Escape:
							bDone = true;					
						default:
							break;
					}
					XLookupString(&event.xkey,keys, sizeof(keys), NULL, NULL);
					switch(keys[0])
					{
						case 'F':
						case 'f':
							if(bFullscreen == false)
							{
								ToggleFullscreen();
								bFullscreen = true;
							}
							else
							{
								ToggleFullscreen();
								bFullscreen = false;
							}
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
				case ButtonPress:
					switch(event.xbutton.button)
					{
						case 1:		//Left Mouse button
							break;
						case 2:		//Middle Mouse button
							break;
						case 3:		//Right Mouse button
							break;
						case 4:		//Mouse Wheel Up
							break;
						case 5:		//Mouse Wheel Down
							break;
						default:
							break;
					}
					break;
				case MotionNotify:		//WM_MOUSE_MOVE
					break;
				case ConfigureNotify:	//WM_SIZE
					winWidth = event.xconfigure.width;
					winHeight = event.xconfigure.height;
					resize(winWidth, winHeight);
					break;
				case Expose:			//WM_PAINT
					break;
				case DestroyNotify:		//WM_DESTORY
					break;
				case 33:				//WM_DESTORY
					bDone = true;
				default:
					break;
			}
        }
        //Update call..
        if (gbAnimation)
                {
                    update();
                }
        display();
	}
	
	uninitialize();
	return(0);
}

void CreateWindow(void)
{
	//function prototypes
	void uninitialize(void);
	
	//variable declarations
    XSetWindowAttributes winAttribs;
    int defaultScreen;
    int styleMask;
    GLXFBConfig *pGLXFBConfig = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo *pTempXVisualInfo = NULL;
    int iNumberOfFBConfigs = 0;

    //1: Initialise Frame Buffer Attributes.
    static int frameBufferAttributes[] = {
    	GLX_X_RENDERABLE, True,
    	GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
    	GLX_RENDER_TYPE, GLX_RGBA_BIT,
    	GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
        GLX_STENCIL_SIZE, 8,
        GLX_DOUBLEBUFFER, True,
        None
    };
	
	//code
    gpDisplay = XOpenDisplay(NULL);
    if(gpDisplay == NULL)
    {
        printf("Error: Unable to open XDisplay.\nExitting Now...\n");
        uninitialize();
        exit(1);
    }

    defaultScreen = XDefaultScreen(gpDisplay);

    //2: Reterive all 'FBConfig's, driver has.
    pGLXFBConfig = glXChooseFBConfig(gpDisplay, defaultScreen, frameBufferAttributes, &iNumberOfFBConfigs);
    printf("Their are %d matching FBConfigs.\n", iNumberOfFBConfigs);

    int bestFrameBufferConfig = -1;
    int bestNumberOfSamples = -1;
    int worstFrameBufferConfig = -1;
    int worstNumberOfSamples = 999;
    for(int i = 0; i<iNumberOfFBConfigs; i++)
    {
    	//3: For each optained FBConfigs get TempVisualInfo.
    	//Its use is just to check the capability of calling following two calls.
    	pTempXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfig[i]);
    	if(pTempXVisualInfo)
    	{
    		//4: Get Number of samplerBuffer from respective FBConfigs.
    		int sampleBuffer, samples;
    		glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLE_BUFFERS, &sampleBuffer);
    		//5: Get Number of samples from respective FBConfigs.
            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfig[i], GLX_SAMPLES, &samples);

            //6: More the Number of samples and sampleBuffer, More Elligible FBConfig is.
            if(bestFrameBufferConfig < 0 || sampleBuffer && samples > bestNumberOfSamples)
            {
            	bestFrameBufferConfig = i;
            	bestNumberOfSamples = samples;
            }
            if(worstFrameBufferConfig < 0 || !sampleBuffer || samples < worstNumberOfSamples)
            {
            	worstFrameBufferConfig = i;
            	worstNumberOfSamples = samples;
            }
    	}
    	XFree(pTempXVisualInfo);
    }

    //7: Now Assign the found best one.
    bestGLXFBConfig = pGLXFBConfig[bestFrameBufferConfig];

    //8: Now Assign the same best to global one.
    gGLXFBConfig = bestGLXFBConfig;

    //9: Free the optained GLXFBConfig Array.
    XFree(pGLXFBConfig);

    //10: Accordingly now get the best Visual.
    gpXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, bestGLXFBConfig);

    if(gpXVisualInfo == NULL)
    {
        printf("Error: Unable to choose visual.\nExitting Now...\n");
        uninitialize();
        exit(1);
    }

    winAttribs.border_pixel = 0;
    winAttribs.border_pixmap = 0;
    winAttribs.background_pixel = BlackPixel(gpDisplay, defaultScreen);
    winAttribs.background_pixmap = 0;
    winAttribs.colormap = XCreateColormap(gpDisplay, XRootWindow(gpDisplay, gpXVisualInfo->screen), gpXVisualInfo->visual, AllocNone);
    winAttribs.event_mask = ExposureMask | VisibilityChangeMask | ButtonPressMask | KeyPressMask | PointerMotionMask | StructureNotifyMask;

    styleMask = CWBorderPixel | CWBackPixel | CWEventMask | CWColormap;
    gColormap = winAttribs.colormap;

    gWindow = XCreateWindow(gpDisplay, XRootWindow(gpDisplay, gpXVisualInfo->screen), 0, 0, giWindowWidth, giWindowHeight, 0, gpXVisualInfo->depth, InputOutput, gpXVisualInfo->visual, styleMask, &winAttribs);

    if(!gWindow)
    {
        printf("Error: Failed to create main window.\nExitting Now...\n");
        uninitialize();
        exit(1);
    }

    XStoreName(gpDisplay, gWindow, "Rohan V. Waghmode - XWindow");

    Atom windowManagerDelete = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(gpDisplay, gWindow, &windowManagerDelete, 1);
	
    XMapWindow(gpDisplay, gWindow);
}

void initialize(void)
{
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;

    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

	//function prototypes
	void uninitialize(void);

	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

    if(glXCreateContextAttribsARB == NULL)
    {
        printf("Error: Unable to get glXCreateContextAttribsARBProc address.\nExitting Now...\n");
        uninitialize();
        exit(1);
    }

    GLint attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 4,
        GLX_CONTEXT_MINOR_VERSION_ARB, 5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);

    // If not optained the  Higher one , Specify the lowest one;
    // And it will give you the highest one possible/known to it.
    if(!gGLXContext)
    {
        GLint attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 1,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };

        gGLXContext = glXCreateContextAttribsARB(gpDisplay, gGLXFBConfig, 0, True, attribs);
    }

    // Check whether the optained context is really hardware rendering context or not...
    if(!glXIsDirect(gpDisplay, gGLXContext))
    {
        printf("The optained context is not hardware rendering context.\n");
    }
    else
    {
        printf("The optained context is hardware rendering context.\n");
    }

    // Make the Optained Context as Current OpenGL Context.
    glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

    GLenum result = glewInit();
	if (result != GLEW_OK)
	{
		printf("Cannot initialize GLEW.\n");
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
    glShaderSource(gVertexShaderObject, 1, (const char**)& vertexShaderSourceCode, NULL);
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
                printf("Vertex Shader Compile Log:\n%s\n", szInfoLog);
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
        "       \n" \
        "       vec3 normalized_light_direction_zero = normalize(light_direction_zero);" \
        "       float tn_dot_ld_zero = max(dot(normalized_light_direction_zero, normalized_tNorm), 0.0);" \
        "       vec3 ambient_zero = u_la_zero * u_ka;" \
        "       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" \
        "       vec3 reflection_vector_zero = reflect(-normalized_light_direction_zero, normalized_tNorm);" \
        "       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, normalized_light_direction_zero), 0.0), u_material_shininess);" \
        "       \n" \
        "       vec3 normalized_light_direction_one = normalize(light_direction_one);" \
        "       float tn_dot_ld_one = max(dot(normalized_light_direction_one, normalized_tNorm), 0.0);" \
        "       vec3 ambient_one = u_la_one * u_ka;" \
        "       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" \
        "       vec3 reflection_vector_one = reflect(-normalized_light_direction_one, normalized_tNorm);" \
        "       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, normalized_light_direction_one), 0.0), u_material_shininess);" \
        "       \n" \
        "       vec3 normalized_light_direction_two = normalize(light_direction_two);" \
        "       float tn_dot_ld_two = max(dot(normalized_light_direction_two, normalized_tNorm), 0.0);" \
        "       vec3 ambient_two = u_la_two * u_ka;" \
        "       vec3 diffuse_two = u_ld_two * u_kd * tn_dot_ld_two;" \
        "       vec3 reflection_vector_two = reflect(-normalized_light_direction_two, normalized_tNorm);" \
        "       vec3 specular_two = u_ls_two * u_ks * pow(max(dot(reflection_vector_two, normalized_light_direction_two), 0.0), u_material_shininess);" \
        "       \n" \
        "       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one + ambient_two + diffuse_two + specular_two;"
        "   }" \
        "   else" \
        "   {" \
        "       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "   FragColor = vec4(phong_ads_light, 1.0);" \
        "}";
    //Specifying Above code to the FragmentShaderObject
    glShaderSource(gFragmentShaderObject, 1, (const char**)& fragmentShaderSourceCode, NULL);
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
                printf("Fragment Shader Compile Log:\n%s\n", szInfoLog);
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
                printf("Shader Program Link Log:\n%s\n", szInfoLog);
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

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    resize(giWindowWidth, giWindowHeight);
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

    glXSwapBuffers(gpDisplay, gWindow);
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

void ToggleFullscreen(void)
{
	//variable declarations
    Atom wm_state;
    Atom fullscreen;
    XEvent xev = {0};

	//code
    wm_state = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
    memset(&xev, 0, sizeof(xev));
	
    xev.type = ClientMessage;
    xev.xclient.window = gWindow;
    xev.xclient.message_type = wm_state;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = bFullscreen ? 0 : 1;
	
	fullscreen = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);
    xev.xclient.data.l[1] = fullscreen;

    XSendEvent(gpDisplay, XRootWindow(gpDisplay, gpXVisualInfo->screen), False, StructureNotifyMask, &xev);
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
	GLXContext currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}

	if(gGLXFBConfig)
	{
		XFree(gGLXFBConfig);
	}

	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}
    if(gWindow)
    {
        XDestroyWindow(gpDisplay, gWindow);
    }

    if(gColormap)
    {
        XFreeColormap(gpDisplay, gColormap);
    }

    if(gpXVisualInfo)
    {
        free(gpXVisualInfo);
        gpXVisualInfo = NULL;
    }

    if(gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay = NULL;
    }
}
