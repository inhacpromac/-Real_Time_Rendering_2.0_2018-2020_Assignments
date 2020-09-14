// Assignment_027
// TessellationShader

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
GLuint gTessellationControlShaderObject;
GLuint gTessellationEvaluationShaderObject;
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
GLuint vbo;

GLuint mUniform;
GLuint vUniform;
GLuint pUniform;
GLuint numberOfSegmentsUniform;
GLuint numberOfStripsUniform;
GLuint lineColorUniform;

unsigned int numberOfLineSegments;

mat4 perspectiveProjectionMatrix;

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialShininess = 128.0f; //50.0f

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
                        case XK_Up:
                            numberOfLineSegments++;
                            if (numberOfLineSegments >= 50)
                            {
                                numberOfLineSegments = 50;
                            }
                            break;
                        case XK_Down:
                            numberOfLineSegments--;
                            if (numberOfLineSegments <= 1)
                            {
                                numberOfLineSegments = 1;
                            }
                            break;					
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
        "in vec2 vPosition;" \
        "void main(void)" \
        "{" \
        "   gl_Position = vec4(vPosition, 0.0, 1.0);" \
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
    ///////////////////////////////////////////////////////////////////////////////////////////////
    //TESSELLATION control shader
    //Define Tessellation Shader Object
    gTessellationControlShaderObject = glCreateShader(GL_TESS_CONTROL_SHADER);

    //Write Tessellation Shader Object
    const GLchar* tessellationControlShaderSourceCode =
        "#version 450 core" \
        "\n" \
        "layout(vertices=4)out;" \
        "uniform int numberOfSegments;" \
        "uniform int numberOfStrips;" \
        "void main(void)" \
        "{" \
        "   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" \
        "   gl_TessLevelOuter[0] = float(numberOfStrips);" \
        "   gl_TessLevelOuter[1] = float(numberOfSegments);" \
        "}";
    //Specifying Above code to the TessellationShaderObject
    glShaderSource(gTessellationControlShaderObject, 1, 
        (const GLchar**) &tessellationControlShaderSourceCode, NULL);
    //Compile the Tessellation Shader
    glCompileShader(gTessellationControlShaderObject);
    //Error Checking for Fragment Shader Compiling
    iInfoLogLength = 0;
    iShaderCompileStatus = 0;
    szInfoLog = NULL;
    glGetShaderiv(gTessellationControlShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gTessellationControlShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gTessellationControlShaderObject, iInfoLogLength, &written, szInfoLog);
                printf("Tessellation Control Shader Compile Log:\n%s\n", szInfoLog);
                free(szInfoLog);
                uninitialize();
                exit(0);
            }
        }
    }

    //TESSELLATION evaluation shader
    //Define Tessellation Shader Object
    gTessellationEvaluationShaderObject = glCreateShader(GL_TESS_EVALUATION_SHADER);

    //Write Tessellation Shader Object
    const GLchar* tessellationEvaluationShaderSourceCode =
        "#version 450" \
        "\n" \
        "layout(isolines)in;" \
        "uniform mat4 u_m_matrix;" \
        "uniform mat4 u_v_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "void main(void)" \
        "{" \
        "   /*Bernstein equation*/" \
        "   float u = gl_TessCoord.x;" \
        "   vec3 p0 = gl_in[0].gl_Position.xyz;" \
        "   vec3 p1 = gl_in[1].gl_Position.xyz;" \
        "   vec3 p2 = gl_in[2].gl_Position.xyz;" \
        "   vec3 p3 = gl_in[3].gl_Position.xyz;" \
        "   float u1 = (1.0 - u);" \
        "   float u2 = u * u;" \
        "   float b3 = u2 * u;" \
        "   float b2 = 3.0 * u2 * u1;" \
        "   float b1 = 3.0 * u * u1 * u1;" \
        "   float b0 = u1 * u1 * u1;" \
        "   vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;" \
        "   gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vec4(p, 1.0);" \
        "}";
    //Specifying Above code to the TessellationShaderObject
    glShaderSource(gTessellationEvaluationShaderObject, 1,
        (const GLchar**)&tessellationEvaluationShaderSourceCode, NULL);
    //Compile the Tessellation Shader
    glCompileShader(gTessellationEvaluationShaderObject);
    //Error Checking for Fragment Shader Compiling
    iInfoLogLength = 0;
    iShaderCompileStatus = 0;
    szInfoLog = NULL;
    glGetShaderiv(gTessellationEvaluationShaderObject, GL_COMPILE_STATUS, &iShaderCompileStatus);
    if (iShaderCompileStatus == GL_FALSE)
    {
        glGetShaderiv(gTessellationEvaluationShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (GLchar*)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gTessellationEvaluationShaderObject, iInfoLogLength, &written, szInfoLog);
                printf("Tessellation Evaluation Shader Compile Log:\n%s\n", szInfoLog);
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
        "#version 410" \
        "\n" \
        "uniform vec4 lineColor;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "   FragColor = lineColor;" \
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
    //Attach tessellation control shader to Shader Program
    glAttachShader(gShaderProgramObject, gTessellationControlShaderObject);
    //Attach tessellation evaluation shader to Shader Program
    glAttachShader(gShaderProgramObject, gTessellationEvaluationShaderObject);
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
    mUniform = glGetUniformLocation(gShaderProgramObject, "u_m_matrix");
    vUniform = glGetUniformLocation(gShaderProgramObject, "u_v_matrix");
    pUniform = glGetUniformLocation(gShaderProgramObject, "u_p_matrix");
    numberOfSegmentsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfSegments");
    numberOfStripsUniform = glGetUniformLocation(gShaderProgramObject, "numberOfStrips");
    lineColorUniform = glGetUniformLocation(gShaderProgramObject, "lineColor");
    ///////////////////////////////////////////////////////////////////////////////////////////////

    const GLfloat vertices[] = {-1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    perspectiveProjectionMatrix = mat4::identity();

    glLineWidth(3.0f);
    numberOfLineSegments = 1; // initially 1 straignt line

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
    translationMatrix = translate(0.0f, 0.0f, -4.0f);

    //Do necessary Matrix Multiplication
    modelMatrix = modelMatrix * translationMatrix;
    projectionMatrix = projectionMatrix * perspectiveProjectionMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(vUniform, 1, GL_FALSE, ViewMatrix);
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, projectionMatrix);

    // send other uniforms
    glUniform1i(numberOfSegmentsUniform, numberOfLineSegments);
    glUniform1i(numberOfStripsUniform, 1);
    glUniform4fv(lineColorUniform, 1, vec4(1.0f, 1.0f, 0.0f, 1.0f));

    //Bind with vao
    glBindVertexArray(vao);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawArrays(GL_PATCHES, 0, 4);

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
    if (vbo)
    {
        glDeleteBuffers(1, &vbo);
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
