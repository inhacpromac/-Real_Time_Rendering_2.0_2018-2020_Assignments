// Assignment_024
// 24 Material Sphere

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

bool gbAnimation = false;
bool gbLighting = false;
bool lightXRotate = false;
bool lightYRotate = false;
bool lightZRotate = false;
GLuint gNumVertices;
GLuint gNumElements;

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
//vao-shapes
//vbo-color,texture,....
GLuint gVao_sphere;
GLuint gVbo_sphere_position;
GLuint gVbo_sphere_normal;
GLuint gVbo_sphere_element;

GLuint mUniform;
GLuint vUniform;
GLuint pUniform;

GLuint laUniform = 0;
GLuint ldUniform = 0;
GLuint lsUniform = 0;
GLuint kaUniform = 0;
GLuint kdUniform = 0;
GLuint ksUniform = 0;
GLuint materialShininessUniform = 0;
GLuint lightPositionUniform = 0;
GLuint lightingEnabledUniform;

mat4 perspectiveProjectionMatrix;

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 1.0f,1.0f,1.0f,1.0f };

GLfloat angleLight = 0.0f;
GLfloat speed = 0.1f;

const int totalNumberOfRows = 6;
const int totalNumberOfColumns = 4;
GLfloat materialAmbient[totalNumberOfColumns][totalNumberOfRows][4] = {
    {
        {0.0215f, 0.1745f, 0.0215f, 1.0f },
        {0.135f, 0.2225f, 0.1575f, 1.0f },
        {0.05375f, 0.05f, 0.06625f, 1.0f },
        {0.25f, 0.20725f, 0.20725f, 1.0f },
        {0.1745f, 0.01175f, 0.01175f, 1.0f },
        {0.1f, 0.18725f, 0.1745f, 1.0f }
    },
    {
        {0.329412f, 0.223529f, 0.027451f, 1.0f },
        {0.2125f, 0.1275f, 0.054f, 1.0f },
        {0.25f, 0.25f, 0.25f, 1.0f },
        {0.19125f, 0.0735f, 0.0225f, 1.0f },
        {0.24725f, 0.1995f, 0.0745f, 1.0f },
        {0.19225f, 0.19225f, 0.19225f, 1.0f }
    },
    {
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.1f, 0.06f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f }
    },
    {
        {0.02f, 0.02f, 0.02f, 1.0f },
        {0.0f, 0.05f, 0.05f, 1.0f },
        {0.0f, 0.05f, 0.0f, 1.0f },
        {0.05f, 0.0f, 0.0f, 1.0f },
        {0.05f, 0.05f, 0.05f, 1.0f },
        {0.05f, 0.05f, 0.0f, 1.0f }
    }
};

GLfloat materialDiffuse[totalNumberOfColumns][totalNumberOfRows][4] = {
    {
        {0.07568f, 0.61424f, 0.07568f, 1.0f},
        {0.54f, 0.89f, 0.63f, 1.0f},
        {0.18275f, 0.17f, 0.22525f, 1.0f},
        {1.0f, 0.829f, 0.829f, 1.0f},
        {0.61424f, 0.04136f, 0.04136f, 1.0f},
        {0.396f, 0.74151f, 0.69102f, 1.0f},
    },
    {
        {0.780392f, 0.568627f, 0.113725f, 1.0f},
        {0.714f, 0.4284f, 0.18144f, 1.0f},
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.7038f, 0.27048f, 0.0828f, 1.0f},
        {0.75164f, 0.60648f, 0.22648f, 1.0f},
        {0.50754f, 0.50754f, 0.50754f, 1.0f},
    },
    {
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.0f, 0.50980392f, 0.50980392f, 1.0f},
        {0.1f, 0.35f, 0.1f, 1.0f},
        {0.5f, 0.0f, 0.0f, 1.0f},
        {0.55f, 0.55f, 0.55f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
    },
    {
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.4f, 0.5f, 0.5f, 1.0f},
        {0.4f, 0.5f, 0.4f, 1.0f},
        {0.5f, 0.4f, 0.4f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.4f, 1.0f},
    },
};

GLfloat materialSpecular[totalNumberOfColumns][totalNumberOfRows][4] = {
    {
        {0.633f, 0.727811f, 0.633f, 1.0f},
        {0.316228f, 0.316228f, 0.316228f, 1.0f},
        {0.332741f, 0.328634f, 0.346435f, 1.0f},
        {0.296648f, 0.296648f, 0.296648f, 1.0f},
        {0.727811f, 0.626959f, 0.626959f, 1.0f},
        {0.297254f, 0.30829f, 0.306678f, 1.0f},
    },
    {
        {0.992157f, 0.941176f, 0.807843f, 1.0f},
        {0.393548f, 0.271906f, 0.166721f, 1.0f},
        {0.774597f, 0.774597f, 0.774597f, 1.0f},
        {0.256777f, 0.137622f, 0.086014f, 1.0f},
        {0.628281f, 0.555802f, 0.366065f, 1.0f},
        {0.508273f, 0.508273f, 0.508273f, 1.0f},
    },
    {
        {0.50f, 0.50f, 0.50f, 1.0f},
        {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
        {0.45f, 0.55f, 0.45f, 1.0f},
        {0.7f, 0.6f, 0.6f, 1.0f},
        {0.70f, 0.70f, 0.70f, 1.0f},
        {0.60f, 0.60f, 0.50f, 1.0f},
    },
    {
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.04f, 0.7f, 0.7f, 1.0f},
        {0.04f, 0.7f, 0.04f, 1.0f},
        {0.7f, 0.04f, 0.04f, 1.0f},
        {0.7f, 0.7f, 0.7f, 1.0f},
        {0.7f, 0.7f, 0.04f, 1.0f},
    }
};

GLfloat materialShininess[totalNumberOfColumns][totalNumberOfRows] = {
    {
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.3f * 128.0f,
        0.088f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f
    },
    {
        0.21794872f * 128.0f,
        0.2f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.4f * 128.0f,
        0.4f * 128.0f
    },
    {
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f
    },
    {
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f
    }
};

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
                        case 'x':
                        case 'X':
                            lightXRotate = true;
                            lightYRotate = false;
                            lightZRotate = false;
                            break;
                        case 'y':
                        case 'Y':
                            lightXRotate = false;
                            lightYRotate = true;
                            lightZRotate = false;
                            break;
                        case 'z':
                        case 'Z':
                            lightXRotate = false;
                            lightYRotate = false;
                            lightZRotate = true;
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
    laUniform = glGetUniformLocation(gShaderProgramObject, "u_la");
    ldUniform = glGetUniformLocation(gShaderProgramObject, "u_ld");
    lsUniform = glGetUniformLocation(gShaderProgramObject, "u_ls");
    kaUniform = glGetUniformLocation(gShaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(gShaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(gShaderProgramObject, "u_ks");
    materialShininessUniform = glGetUniformLocation(gShaderProgramObject, "u_material_shininess");
    lightPositionUniform = glGetUniformLocation(gShaderProgramObject, "u_light_position");
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

    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    resize(giWindowWidth, giWindowHeight);
}

int vWidth = 1;
int vHeight = 1;
int vX = 0;
int vY = 0;

void resize(int width, int height)
{
	if (height == 0)
    {
        height = 1;
    }
    vWidth = width / totalNumberOfRows;
    vHeight = height / totalNumberOfRows;
    vX = (width - (vWidth * totalNumberOfColumns)) / 2;
    vY = (height - (vHeight * totalNumberOfRows)) / 2;
    perspectiveProjectionMatrix = perspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int column = 0; column < totalNumberOfColumns; ++column)
    {
        for (int row = 0; row < totalNumberOfRows; ++row)
        {
            glViewport(vX + (column * vWidth), vY + (row * vHeight), vWidth, vHeight);

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
                glUniform3fv(laUniform, 1, lightAmbient);
                glUniform3fv(ldUniform, 1, lightDiffuse);
                glUniform3fv(lsUniform, 1, lightSpecular);
                glUniform3fv(kaUniform, 1, materialAmbient[column][row]);
                glUniform3fv(kdUniform, 1, materialDiffuse[column][row]);
                glUniform3fv(ksUniform, 1, materialSpecular[column][row]);
                glUniform1f(materialShininessUniform, materialShininess[column][row]);
                glUniform4fv(lightPositionUniform, 1, lightPosition);
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

        }
    }

    glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
    angleLight += speed;

    if (angleLight >= 360.0f)
    {
        angleLight = 0.0f;
    }
    if (lightXRotate)
    {
        lightPosition[0] = 0.0f;
        lightPosition[1] = sinf(angleLight) * 100.0f - 3.0f;
        lightPosition[2] = cosf(angleLight) * 100.0f - 3.0f;
    }
    else if (lightYRotate)
    {
        lightPosition[0] = sinf(angleLight) * 100.0f - 3.0f;
        lightPosition[1] = 0.0f;
        lightPosition[2] = cosf(angleLight) * 100.0f - 3.0f;
    }
    else if (lightZRotate)
    {
        lightPosition[0] = sinf(angleLight) * 100.0f - 3.0f;
        lightPosition[1] = cosf(angleLight) * 100.0f - 3.0f;
        lightPosition[2] = 0.0f;
    }
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
