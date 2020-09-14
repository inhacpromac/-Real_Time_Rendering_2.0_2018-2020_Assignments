//Assignment_021
//24 Spheres

//header
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

//namespaces
using namespace std;

//global variable declarations
bool bFullscreen = false;
Display *gpDisplay = NULL;
XVisualInfo *gpXVisualInfo = NULL;
Colormap gColormap;
Window gWindow;
int giWindowWidth = 800;
int giWindowHeight = 600;

GLXContext gGLXContext;

GLfloat spinning_angle = 0.0f;

//Global variable declaration of toggling light
bool bLight = false;
GLfloat angleOfXRotation = 0.0f;
GLfloat angleOfYRotation = 0.0f;
GLfloat angleOfZRotation = 0.0f;
GLint keyPressed = 0;

//Global Declaration of light configuration parameter
GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLfloat light_model_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat light_model_local_viewer[] = { 0.0f };

GLUquadric *Quadric[24];

void resize(int, int);
void update(void);
void draw24Spheres();

//entry point function
int main(void)
{
	//function prototypes
	void CreateWindow(void);
	void ToggleFullscreen(void);
	void uninitialize();
	void initialize(void);
	
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
						case 'L':
						case 'l':
							if (bLight == false)
							{
								bLight = true;
								glEnable(GL_LIGHTING);
							}
							else
							{
								bLight = false;
								glDisable(GL_LIGHTING);
							}
							break;
						case 'X':
						case 'x':
							keyPressed = 1;
							angleOfXRotation = 0.0f;
							break;
						case 'Y':
						case 'y':
							keyPressed = 2;
							angleOfYRotation = 0.0f;
							break;
						case 'Z':
						case 'z':
							keyPressed = 3;
							angleOfZRotation = 0.0f;
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
        update();
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
    static int frameBufferAttributes[] = {
    	GLX_RGBA,
    	GLX_DOUBLEBUFFER, True,
        GLX_RED_SIZE, 8,
        GLX_GREEN_SIZE, 8,
        GLX_BLUE_SIZE, 8,
        GLX_ALPHA_SIZE, 8,
        GLX_DEPTH_SIZE, 24,
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
	
    gpXVisualInfo = glXChooseVisual(gpDisplay, defaultScreen, frameBufferAttributes);
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
	//function prototypes
	void uninitialize(void);

	gGLXContext = glXCreateContext(gpDisplay, gpXVisualInfo, NULL, GL_TRUE);
	glXMakeCurrent(gpDisplay, gWindow, gGLXContext);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glShadeModel(GL_SMOOTH);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, light_model_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, light_model_local_viewer);
	glEnable(GL_LIGHT0);

	for (int i = 0; i < 24; i++)
	{
		Quadric[i] = gluNewQuadric();
	}

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
    resize(giWindowWidth, giWindowHeight);
}

void resize(int width, int height)
{
	 if (height == 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (width <= height)
	{
		glOrtho(0.0f, 15.5f, 0.0f, 15.5f*GLfloat(height) / GLfloat(width), -10.0f, 10.0f);
	}
	else
	{
		glOrtho(0.0f, 15.5f*GLfloat(width) / GLfloat(height), 0.0f, 15.5f, -10.0f, 10.0f);
	}
}

void draw24Spheres()
{
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat materialShininess[1];

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// ***** 1st sphere on 1st column, emerald *****
	materialAmbient[0] = { 0.0215f };
	materialAmbient[1] = { 0.1745f };
	materialAmbient[2] = { 0.0215f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.07568f };
	materialDiffuse[1] = { 0.61424f };
	materialDiffuse[2] = { 0.07568f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.633f };
	materialSpecular[1] = { 0.727811f };
	materialSpecular[2] = { 0.633f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.6f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 14.0f, 0.0f);
	gluSphere(Quadric[0], 1.0f, 30, 30);

	// ***** 2nd sphere on 1st column, jade *****
	materialAmbient[0] = { 0.135f };
	materialAmbient[1] = { 0.2225f };
	materialAmbient[2] = { 0.1575f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.54f };
	materialDiffuse[1] = { 0.89f };
	materialDiffuse[2] = { 0.63f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.316228f };
	materialSpecular[1] = { 0.316228f };
	materialSpecular[2] = { 0.316228f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.1f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 11.5f, 0.0f);
	gluSphere(Quadric[1], 1.0f, 30, 30);

	// ***** 3rd sphere on 1st column, obsidian *****
	materialAmbient[0] = { 0.05375f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.06625f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.18275f };
	materialDiffuse[1] = { 0.17f };
	materialDiffuse[2] = { 0.22525f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.332741f };
	materialSpecular[1] = { 0.328634f };
	materialSpecular[2] = { 0.346435f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.3f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 9.0f, 0.0f);
	gluSphere(Quadric[2], 1.0f, 30, 30);

	// ***** 4th sphere on 1st column, pearl *****
	materialAmbient[0] = { 0.25f };
	materialAmbient[1] = { 0.20725f };
	materialAmbient[2] = { 0.20725f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 1.0f };
	materialDiffuse[1] = { 0.829f };
	materialDiffuse[2] = { 0.829f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.296648f };
	materialSpecular[1] = { 0.296648f };
	materialSpecular[2] = { 0.296648f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.088f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 6.5f, 0.0f);
	gluSphere(Quadric[3], 1.0f, 30, 30);

	// ***** 5th sphere on 1st column, ruby *****
	materialAmbient[0] = { 0.1745f };
	materialAmbient[1] = { 0.01175f };
	materialAmbient[2] = { 0.01175f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.61424f };
	materialDiffuse[1] = { 0.04136f };
	materialDiffuse[2] = { 0.04136f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.727811f };
	materialSpecular[1] = { 0.626959f };
	materialSpecular[2] = { 0.626959f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.6f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 4.0f, 0.0f);
	gluSphere(Quadric[4], 1.0f, 30, 30);

	// ***** 6th sphere on 1st column, turquoise *****
	materialAmbient[0] = { 0.1f };
	materialAmbient[1] = { 0.18725f };
	materialAmbient[2] = { 0.1745f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.396f };
	materialDiffuse[1] = { 0.74151f };
	materialDiffuse[2] = { 0.69102f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.297254f };
	materialSpecular[1] = { 0.30829f };
	materialSpecular[2] = { 0.306678f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.1f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(1.5f, 1.5f, 0.0f);
	gluSphere(Quadric[5], 1.0f, 30, 30);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ***** 1st sphere on 2nd column, brass *****
	materialAmbient[0] = { 0.329412f };
	materialAmbient[1] = { 0.223529f };
	materialAmbient[2] = { 0.027451f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.780392f };
	materialDiffuse[1] = { 0.568627f };
	materialDiffuse[2] = { 0.113725f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.992157f };
	materialSpecular[1] = { 0.941176f };
	materialSpecular[2] = { 0.807843f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.21794872f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 14.0f, 0.0f);
	gluSphere(Quadric[6], 1.0f, 30, 30);

	// ***** 2nd sphere on 2nd column, bronze *****
	materialAmbient[0] = { 0.2125f };
	materialAmbient[1] = { 0.1275f };
	materialAmbient[2] = { 0.054f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.714f };
	materialDiffuse[1] = { 0.4284f };
	materialDiffuse[2] = { 0.18144f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.393548f };
	materialSpecular[1] = { 0.271906f };
	materialSpecular[2] = { 0.166721f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.2f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 11.5f, 0.0f);
	gluSphere(Quadric[7], 1.0f, 30, 30);

	// ***** 3rd sphere on 2nd column, chrome *****
	materialAmbient[0] = { 0.25f };
	materialAmbient[1] = { 0.25f };
	materialAmbient[2] = { 0.25f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.4f };
	materialDiffuse[1] = { 0.4f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.774597f };
	materialSpecular[1] = { 0.774597f };
	materialSpecular[2] = { 0.774597f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.6f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 9.0f, 0.0f);
	gluSphere(Quadric[8], 1.0f, 30, 30);

	// ***** 4th sphere on 2nd column, copper *****
	materialAmbient[0] = { 0.19125f };
	materialAmbient[1] = { 0.0735f };
	materialAmbient[2] = { 0.0225f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.7038f };
	materialDiffuse[1] = { 0.27048f };
	materialDiffuse[2] = { 0.0828f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.256777f };
	materialSpecular[1] = { 0.1376229f };
	materialSpecular[2] = { 0.086014f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.1f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 6.5f, 0.0f);
	gluSphere(Quadric[9], 1.0f, 30, 30);

	// ***** 5th sphere on 2nd column, gold *****
	materialAmbient[0] = { 0.24725f };
	materialAmbient[1] = { 0.1995f };
	materialAmbient[2] = { 0.0745f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.75164f };
	materialDiffuse[1] = { 0.60648f };
	materialDiffuse[2] = { 0.22648f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.628281f };
	materialSpecular[1] = { 0.555802f };
	materialSpecular[2] = { 0.366065f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.4f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 4.0f, 0.0f);
	gluSphere(Quadric[10], 1.0f, 30, 30);

	// ***** 6th sphere on 2nd column, silver *****
	materialAmbient[0] = { 0.19225f };
	materialAmbient[1] = { 0.19225f };
	materialAmbient[2] = { 0.19225f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.50754f };
	materialDiffuse[1] = { 0.50754f };
	materialDiffuse[2] = { 0.50754f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.508273f };
	materialSpecular[1] = { 0.508273f };
	materialSpecular[2] = { 0.508273f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.4f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(5.5f, 1.5f, 0.0f);
	gluSphere(Quadric[11], 1.0f, 30, 30);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ***** 1st sphere on 3rd column, black *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.01f };
	materialDiffuse[1] = { 0.01f };
	materialDiffuse[2] = { 0.01f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.50f };
	materialSpecular[1] = { 0.50f };
	materialSpecular[2] = { 0.50f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 14.0f, 0.0f);
	gluSphere(Quadric[12], 1.0f, 30, 30);

	// ***** 2nd sphere on 3rd column, cyan *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.1f };
	materialAmbient[2] = { 0.06f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.0f };
	materialDiffuse[1] = { 0.50980392f };
	materialDiffuse[2] = { 0.50980392f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.50196078f };
	materialSpecular[1] = { 0.50196078f };
	materialSpecular[2] = { 0.50196078f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 11.5f, 0.0f);
	gluSphere(Quadric[13], 1.0f, 30, 30);

	// ***** 3rd sphere on 3rd column, green *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.1f };
	materialDiffuse[1] = { 0.35f };
	materialDiffuse[2] = { 0.1f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.45f };
	materialSpecular[1] = { 0.55f };
	materialSpecular[2] = { 0.45f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 9.0f, 0.0f);
	gluSphere(Quadric[14], 1.0f, 30, 30);

	// ***** 4th sphere on 3rd column, red *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.0f };
	materialDiffuse[2] = { 0.0f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.6f };
	materialSpecular[2] = { 0.6f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 6.5f, 0.0f);
	gluSphere(Quadric[15], 1.0f, 30, 30);

	// ***** 5th sphere on 3rd column, white *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.55f };
	materialDiffuse[1] = { 0.55f };
	materialDiffuse[2] = { 0.55f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.70f };
	materialSpecular[1] = { 0.70f };
	materialSpecular[2] = { 0.70f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 4.0f, 0.0f);
	gluSphere(Quadric[16], 1.0f, 30, 30);

	// ***** 6th sphere on 3rd column, yellow plastic *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.0f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.60f };
	materialSpecular[1] = { 0.60f };
	materialSpecular[2] = { 0.50f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.25f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(9.5f, 1.5f, 0.0f);
	gluSphere(Quadric[17], 1.0f, 30, 30);
	////////////////////////////////////////////////////////////////////////////////////////////////
	// ***** 1st sphere on 4th column, black *****
	materialAmbient[0] = { 0.02f };
	materialAmbient[1] = { 0.02f };
	materialAmbient[2] = { 0.02f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.01f };
	materialDiffuse[1] = { 0.01f };
	materialDiffuse[2] = { 0.01f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.4f };
	materialSpecular[1] = { 0.4f };
	materialSpecular[2] = { 0.4f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 14.0f, 0.0f);
	gluSphere(Quadric[18], 1.0f, 30, 30);

	// ***** 2nd sphere on 4th column, cyan *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.05f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.4f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.5f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.04f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.7f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 11.5f, 0.0f);
	gluSphere(Quadric[19], 1.0f, 30, 30);

	// ***** 3rd sphere on 4th column, green *****
	materialAmbient[0] = { 0.0f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.4f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.04f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.04f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 9.0f, 0.0f);
	gluSphere(Quadric[20], 1.0f, 30, 30);

	// ***** 4th sphere on 4th column, red *****
	materialAmbient[0] = { 0.05f };
	materialAmbient[1] = { 0.0f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.4f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.04f };
	materialSpecular[2] = { 0.04f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125 * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 6.5f, 0.0f);
	gluSphere(Quadric[21], 1.0f, 30, 30);

	// ***** 5th sphere on 4th column, white *****
	materialAmbient[0] = { 0.05f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.05f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.5f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.7f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 4.0f, 0.0f);
	gluSphere(Quadric[22], 1.0f, 30, 30);

	// ***** 6th sphere on 4th column, yellow rubber *****
	materialAmbient[0] = { 0.05f };
	materialAmbient[1] = { 0.05f };
	materialAmbient[2] = { 0.0f };
	materialAmbient[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
	materialDiffuse[0] = { 0.5f };
	materialDiffuse[1] = { 0.5f };
	materialDiffuse[2] = { 0.4f };
	materialDiffuse[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	materialSpecular[0] = { 0.7f };
	materialSpecular[1] = { 0.7f };
	materialSpecular[2] = { 0.04f };
	materialSpecular[3] = { 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	materialShininess[0] = { 0.078125f * 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(13.5f, 1.5f, 0.0f);
	gluSphere(Quadric[23], 1.0f, 30, 30);
}

void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	if (keyPressed == 1)
	{
		glRotatef(angleOfXRotation, 1.0f, 0.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = angleOfXRotation;
		lightPosition[2] = 0.0f;
	}
	else if (keyPressed == 2)
	{
		glRotatef(angleOfYRotation, 0.0f, 1.0f, 0.0f);
		lightPosition[0] = 0.0f;
		lightPosition[1] = 0.0f;
		lightPosition[2] = angleOfYRotation;
	}
	else if (keyPressed == 3)
	{
		glRotatef(angleOfZRotation, 0.0f, 0.0f, 1.0f);
		lightPosition[0] = angleOfZRotation;
		lightPosition[1] = 0.0f;
		lightPosition[2] = 0.0f;
	}
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	draw24Spheres();


	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	if (angleOfXRotation >= 360.0f)
	{
		angleOfXRotation = 0.0f;
	}
	else
	{
		angleOfXRotation = angleOfXRotation + 1.0f;
	}
	if (angleOfYRotation >= 360.0f)
	{
		angleOfYRotation = 0.0f;
	}
	else
	{
		angleOfYRotation = angleOfYRotation + 1.0f;
	}
	if (angleOfZRotation >= 360.0f)
	{
		angleOfZRotation = 0.0f;
	}
	else
	{
		angleOfZRotation = angleOfZRotation + 1.0f;
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
	GLXContext currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}
	if (Quadric)
	{
		for (int i = 0; i < 24; i++)
		{
			gluDeleteQuadric(Quadric[i]);
		}
		Quadric[24];
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
