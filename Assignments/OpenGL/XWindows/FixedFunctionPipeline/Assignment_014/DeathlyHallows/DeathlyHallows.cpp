//Assignment_014
//Deathly Hallows Craetion

//header
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

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

GLfloat X1 = 0.0f, Y1 = 0.5f;
GLfloat X2 = -0.5f, Y2 = -0.5f;
GLfloat X3 = 0.5f, Y3 = -0.5f;
GLfloat angle = 0.0f;
GLfloat spinning_angle = 0.0f;
GLfloat radius = 0.0f;
GLfloat s = 0.0f;
GLfloat a = 0.0f;
GLfloat b = 0.0f;
GLfloat c = 0.0f;
GLfloat Xc = 0.0f, Yc = 0.0f;

GLfloat positionOfTriangle = 2.0f;
GLfloat positionOfCircleX = -2.0f;
GLfloat positionOfCircleY = -2.2f;
GLfloat positionOfLine = 2.0f;
GLint isMovementOfTriangleDone = 0, isMovementOfCircleDone = 0, isMovementOfLineDone = 0, isDone = 0;


void resize(int, int);
void update(void);
void drawTriangle();
void drawCircle();
void drawLine();

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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
}

void drawTriangle()
{
	glBegin(GL_LINE_LOOP);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(X1, Y1);
	glVertex2f(X2, Y2);
	glVertex2f(X3, Y3);

	glEnd();
}

void drawCircle()
{
	glBegin(GL_LINE_LOOP);

	for (angle = 0.0f; angle < 2 * M_PI; angle += 0.05f)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2f((GLfloat)(cos(angle)*radius), (GLfloat)(sin(angle)*radius));
	}

	glEnd();
}

void drawLine()
{
	glBegin(GL_LINES);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.0f, -0.5f);

	glEnd();
}

void display(void)
{
	//Distance of Sides of Triangle
	a = (GLfloat)sqrt((X2 - X1)*(X2 - X1) + (Y2 - Y1)*(Y2 - Y1));
	b = (GLfloat)sqrt((X3 - X2)*(X3 - X2) + (Y3 - Y2)*(Y3 - Y2));
	c = (GLfloat)sqrt((X1 - X3)*(X1 - X3) + (Y1 - Y3)*(Y1 - Y3));

	s = (a + b + c) / 2;

	//Radius of Incircle
	radius = (GLfloat)sqrt(s*(s - a)*(s - b)*(s - c)) / s;

	//Centers of Incircle
	Xc = ((b*X1) + (c*X2) + (a*X3)) / (a + b + c);
	Yc = ((b*Y1) + (c*Y2) + (a*Y3)) / (a + b + c);


	//Conditions
	if (isMovementOfTriangleDone == 0 && positionOfTriangle <= 0.0f)
	{
		isMovementOfTriangleDone = 1;
		positionOfTriangle = 0.0f;
	}
	if (isMovementOfCircleDone == 0 && (positionOfCircleX >= Xc) && (positionOfCircleY >= Yc))
	{
		isMovementOfCircleDone = 1;
		positionOfCircleX = Xc;
		positionOfCircleY = Yc;
	}
	if ((isMovementOfLineDone == 0 && positionOfLine <= 0.0f))
	{
		isMovementOfLineDone = 1;
		positionOfLine = 0.0f;
	}
		

	//MovementTowardCenter
	if (isMovementOfTriangleDone != 1)
	{
		positionOfTriangle = positionOfTriangle - 0.01f;
	}
	if (isMovementOfCircleDone != 1)
	{
		positionOfCircleX = positionOfCircleX + 0.01f;
		positionOfCircleY = positionOfCircleY + 0.01f;
	}
	if (isMovementOfCircleDone == 1 && isMovementOfTriangleDone == 1 && isMovementOfLineDone != 1)
	{
		positionOfLine = positionOfLine - 0.01f;
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(positionOfTriangle, (-positionOfTriangle), -3.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawTriangle();

	glLoadIdentity();	
	glTranslatef(positionOfCircleX, positionOfCircleY, -3.0f);
	glRotatef(spinning_angle, 0.0f, 1.0f, 0.0f);

	drawCircle();

	glLoadIdentity();
	glTranslatef(0.0f, positionOfLine, -3.0f);

	drawLine();


	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	spinning_angle = spinning_angle + 1.0f;
	if(spinning_angle>360.0f)
	{
		spinning_angle = 0.0f;
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
