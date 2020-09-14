// Assignment_015
// MeshShapes

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

GLuint gShaderProgramObject;
enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0,
};
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
    GLuint gVertexShaderObject;
    GLuint gFragmentShaderObject;

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
        "in vec4 vColor;" \
        "uniform mat4 u_mvp_matrix;" \
        "out vec4 out_color;" \
        "void main(void)" \
        "{" \
        "   gl_Position = u_mvp_matrix * vPosition;" \
        "   out_color = vColor;" \
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
        "in vec4 out_color;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "   FragColor = out_color;" \
        "}";
    //Specifying A
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
    /*glGenVertexArrays(1, &vao_color_square);
    glBindVertexArray(vao_color_square);
    //Color Squares
    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);*/

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
