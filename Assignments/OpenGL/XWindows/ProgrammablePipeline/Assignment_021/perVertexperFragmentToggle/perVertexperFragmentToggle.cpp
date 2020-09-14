// Assignment_021
// perVertex perFragment Toggle

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
GLfloat materialShininess = 128.0f; //50.0f

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];

void resize(int, int);

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
						case 'Q':
						case 'q':
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
                        case 'v':
                        case 'V':
                            isPerVertexLightEnabled = true;
                            break;
                        case 'f':
                        case 'F':
                            isPerVertexLightEnabled = false;
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
    GLuint gVertexShaderObjectPerVertex;
    GLuint gFragmentShaderObjectPerVertex;

    GLuint gVertexShaderObjectPerFragment;
    GLuint gFragmentShaderObjectPerFragment;

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
                printf("Vertex Shader Compile Log:\n%s\n", szInfoLog);
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
                printf("Fragment Shader Compile Log:\n%s\n", szInfoLog);
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
                printf("Shader Program Link Log:\n%s\n", szInfoLog);
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
                printf("Vertex Shader Compile Log:\n%s\n", szInfoLog);
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
        "   FragColor = vec4(phong_ads_light, 1.0);" \
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
                printf("Fragment Shader Compile Log:\n%s\n", szInfoLog);
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
                printf("Shader Program Link Log:\n%s\n", szInfoLog);
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

    glXSwapBuffers(gpDisplay, gWindow);
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
