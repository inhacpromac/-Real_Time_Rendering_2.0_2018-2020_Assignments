//Assignment_016
//Dynamic INDIA

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

GLint moveI1 = 0, moveN = 0, moveD = 0, moveA = 0, moveI2 = 0, moveP1 = 0, moveP2 = 0, moveP3 = 0;
GLfloat posI1 = -9.0f;
GLfloat posN = -6.0f;
GLfloat posI2 = 6.0f;
GLfloat posA = 9.0f;
GLfloat posDOR = 0.0f; // red color of D to achive orange color
GLfloat posDOG = 0.0f; // green color of D to achive orange color
GLfloat posDOB = 0.0f; // blue color of D to achive orange color
GLfloat posDGR = 0.0f; // red color of D to achive green color
GLfloat posDGG = 0.0f; // green color of D to achive green color
GLfloat posDGB = 0.0f; // blue color of D to achive green color
GLfloat posP1X = -9.0f;
GLfloat posP1Y = 6.0f;
GLfloat posP2X = -9.0f;
GLfloat posP2Y = 0.5f;
GLfloat posP3X = -9.0f;
GLfloat posP3Y = -5.0f;
GLfloat posFlag = -101.0f;

void resize(int, int);
void update(void);
void drawI();
void drawN();
void drawD();
void drawA();
void drawFlag();

struct Plane
{
	GLfloat top = 0.0f;
	GLfloat bottom = 0.0f;
	GLfloat left = 0.0f;
	GLfloat right = 0.0f;
	//GLfloat negLeft = 0.0f;
	//GLfloat negRight = 0.0f;
	//GLfloat R = 0.0f;
	//GLfloat G = 0.0f;
	//GLfloat B = 0.0f;
};

void drawPlane(struct Plane);

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



void display(void)
{
	//Conditions
	if (moveI1 == 0 && posI1 >= -3.5f)
	{
		moveI1 = 1;
	}
	if (moveA == 0 && posA <= 1.9f)
	{
		moveA = 1;
	}
	if (moveN == 0 && posN >= 0.0f)
	{
		moveN = 1;
	}
	if (moveI2 == 0 && posI2 <= 0.0f)
	{
		moveI2 = 1;
	}
	if (moveD == 0 && posDOR >= 1.0f && posDOG >= 0.5f && posDGG >= 1.0f)
	{
		moveD = 1;
	}


	//Phase1
	if (moveP1 == 0 && posP1X >= -3.5f && posP1Y <= 0.5f)
	{
		moveP1 = 1;
	}
	if (moveP2 == 0 && posP2X >= -3.5f)
	{
		moveP2 = 1;
	}
	if (moveP3 == 0 && posP3X >= -3.5f && posP3Y >= 0.5f)
	{
		moveP3 = 1;
	}


	//Phase2
	if (moveP1 == 1 && posP1X >= 3.3f && posP1Y <= 0.5f)
	{
		moveP1 = 2;
		posFlag = -10.0f;
	}
	if (moveP2 == 1 && posP2X >= 3.3f)
	{
		moveP2 = 2;
	}
	if (moveP3 == 1 && posP3X >= 3.3f && posP3Y >= 0.5f)
	{
		moveP3 = 2;
	}


	//Phase3
	if (moveP1 == 2 && posP1X >= 9.0f && posP1Y <= 0.5f)
	{
		moveP1 = 3;
	}
	if (moveP2 == 2 && posP2X >= 9.0f)
	{
		moveP2 = 3;
	}
	if (moveP3 == 2 && posP3X >= 9.0f && posP3Y >= 0.5f)
	{
		moveP3 = 3;
	}


	//Movement
	if (moveI1 != 1)
	{
		posI1 = posI1 + 0.01f;
	}
	if (moveI1 == 1 && moveA != 1)
	{
		posA = posA - 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN != 1)
	{
		posN = posN + 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 != 1)
	{
		posI2 = posI2 - 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD != 1)
	{
		posDOR = posDOR + 0.01f;
		posDOG = posDOG + 0.005f;  // As we need to reach the value of 0.5f we need to increment accordingly. If we dont do this then we get Yellow Color
		posDGG = posDGG + 0.01f;
	}


	//Phase1
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP1 != 1 && moveP1 < 1)
	{
		posP1X = posP1X + 0.01f;
		posP1Y = posP1Y - 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP2 != 1 && moveP2 < 1)
	{
		posP2X = posP2X + 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP3 != 1 && moveP3 < 1)
	{
		posP3X = posP3X + 0.01f;
		posP3Y = posP3Y + 0.01f;
	}
	
	
	//Phase2
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP1 != 2 && moveP1 >= 1 && moveP1 < 2)
	{
		posP1X = posP1X + 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP2 != 2 && moveP1 >= 1 && moveP2 < 2)
	{
		posP2X = posP2X + 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP3 != 2 && moveP1 >= 1 && moveP3 < 2)
	{
		posP3X = posP3X + 0.01f;
	}
	
	
	//Phase3
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP1 != 3 && moveP1 >= 2 && moveP1 < 3)
	{
		posP1X = posP1X + 0.01f;
		posP1Y = posP1Y + 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP2 != 3 && moveP1 >= 2 && moveP2 < 3)
	{
		posP2X = posP2X + 0.01f;
	}
	if (moveI1 == 1 && moveA == 1 && moveN == 1 && moveI2 == 1 && moveD == 1 && moveP3 != 3 && moveP1 >= 2 && moveP3 < 3)
	{
		posP3X = posP3X + 0.01f;
		posP3Y = posP3Y - 0.01f;
	}
	

	struct Plane Plane1;
	Plane1.top = 0.0f;
	Plane1.bottom = -1.0f;
	Plane1.left = 0.0f;
	Plane1.right = 1.5f;


	struct Plane Plane2;
	Plane2.top = 0.0f;
	Plane2.bottom = -1.0f;
	Plane2.left = 0.0f;
	Plane2.right = 1.5f;

	struct Plane Plane3;
	Plane3.top = 0.0f;
	Plane3.bottom = -1.0f;
	Plane3.left = 0.0f;
	Plane3.right = 1.5f;	
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(posI1, 0.0f, -10.0f);

	drawI();

	glLoadIdentity();
	glTranslatef(-2.75f, posN, -10.0f);

	drawN();

	glLoadIdentity();
	glTranslatef(-0.6f, 0.0f, -10.0f);

	drawD();

	glLoadIdentity();
	glTranslatef(1.15f, posI2, -10.0f);

	drawI();

	glLoadIdentity();
	glTranslatef(posA, 0.0f, -10.0f);

	drawA();

	glLoadIdentity();
	glTranslatef(1.9f, 0.0f, posFlag);

	drawFlag();

	glLoadIdentity();
	glTranslatef(posP1X, posP1Y, -10.0f);

	drawPlane(Plane1);

	glLoadIdentity();
	glTranslatef(posP3X, posP3Y, -10.0f);

	drawPlane(Plane3);

	glLoadIdentity();
	glTranslatef(posP2X, posP2Y, -10.0f);

	drawPlane(Plane2);	


	glXSwapBuffers(gpDisplay, gWindow);
}

void update(void)
{
	
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

void drawI()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);
	glEnd();
}

void drawN()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.4f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.4f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.0f, 1.5f);

	/*glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 0.6f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.8f, -1.5f);

	/*glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -0.6f);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(0.0f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.4f, -1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.8f, -1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.8f, 0.0f);*/

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.8f, 1.5f);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.5f);

	/*glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.4f, 0.0f);*/

	glEnd();
}

void drawD()
{
	glBegin(GL_POLYGON);

	glColor3f(posDOR, posDOG, posDOB);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.5f);

	glColor3f(posDGR, posDGG, posDGB);
	glVertex2f(0.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);	

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(posDGR, posDGG, posDGB);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);
	glVertex2f(0.4f, -1.1f);
	glVertex2f(1.0f, -1.1f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(posDGR, posDGG, posDGB);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(1.4f, -1.1f);

	glColor3f(posDOR, posDOG, posDOB);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(posDOR, posDOG, posDOB);
	glVertex2f(1.0f, 1.5f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.4f, 1.1f);
	glVertex2f(1.0f, 1.1f);

	glEnd();
}

void drawA()
{
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.1f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(0.4f, 1.5f);
	glVertex2f(0.0f, 1.1f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(0.0f, -1.5f);
	glVertex2f(0.4f, -1.5f);

	glEnd();

	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.4f, 1.1f);
	glVertex2f(1.0f, 1.5f);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -1.5f);
	glVertex2f(1.4f, -1.5f);

	glEnd();
}

void drawPlane(struct Plane P)
{
	// Orange
	glBegin(GL_QUADS);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(P.left, P.top - 0.35f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.35f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.45f);
	
	//glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(P.left, P.top - 0.45f);

	glEnd();

	// White
	glBegin(GL_QUADS);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(P.left, P.top - 0.45f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.45f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.55f);
	
	//glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(P.left, P.top - 0.55f);

	glEnd();

	// Green
	glBegin(GL_QUADS);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(P.left, P.top - 0.55f);

	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.55f);
	
	//glColor3f(0.0f, 0.0f, 0.0f);
	glVertex2f(P.left - 1.0f, P.top - 0.65f);
	
	//glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(P.left, P.top - 0.65f);

	glEnd();

	// Plane
	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 0.1f, P.top - 0.35f);
	glVertex2f(P.left, P.top - 0.35f);
	glVertex2f(P.left, P.top - 0.65f);
	glVertex2f(P.left + 0.1f, P.top - 0.65f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 0.8f, P.top - 0.25f);
	glVertex2f(P.left + 0.1f, P.top);
	glVertex2f(P.left + 0.1f, P.bottom);
	glVertex2f(P.left + 0.8f, P.top - 0.75f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 1.0f, P.top - 0.25f);
	glVertex2f(P.left + 0.8f, P.top - 0.25f);
	glVertex2f(P.left + 0.8f, P.top - 0.75f);
	glVertex2f(P.left + 1.0f, P.top - 0.75f);

	glEnd();

	glBegin(GL_QUADS);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.left + 1.25f, P.top - 0.4f);
	glVertex2f(P.left + 1.0f, P.top - 0.4f);
	glVertex2f(P.left + 1.0f, P.top - 0.6f);
	glVertex2f(P.left + 1.25f, P.top - 0.6f);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.right, P.top - 0.5f);
	glVertex2f(P.left + 1.25f, P.top - 0.4f);
	glVertex2f(P.left + 1.25f, P.top - 0.6f);

	glEnd();

	glBegin(GL_TRIANGLES);

	glColor3f(0.73f, 0.89f, 0.93f);
	glVertex2f(P.right, P.top - 0.5f);
	glVertex2f(P.left + 1.25f, P.top - 0.4f);
	glVertex2f(P.left + 1.25f, P.top - 0.6f);

	glEnd();

	glLineWidth(4.0f);
	glBegin(GL_LINES);

	glColor3f(0.698f, 0.133f, 0.133f);
	glVertex2f(P.left + 0.25f, P.top - 0.25f);
	glVertex2f(P.left + 0.25f, P.top - 0.75f);

	glEnd();

	glBegin(GL_LINES);

	glColor3f(0.698f, 0.133f, 0.133f);
	glVertex2f(P.left + 0.25f, P.top - 0.25f);
	glVertex2f(P.left + 0.25f, P.top - 0.75f);

	glEnd();
}

void drawFlag()
{
	//OrangeFlag
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 0.5f, 0.0f);
	glVertex2f(1.0f, 0.15f);
	glVertex2f(0.4f, 0.15f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(1.0f, 0.05f);

	glEnd();

	//WhiteFlag
	glBegin(GL_POLYGON);

	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2f(1.0f, 0.05f);
	glVertex2f(0.4f, 0.05f);
	glVertex2f(0.4f, -0.05f);
	glVertex2f(1.0f, -0.05f);

	glEnd();

	//GreenFlag
	glBegin(GL_POLYGON);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(1.0f, -0.05f);
	glVertex2f(0.4f, -0.05f);
	glVertex2f(0.4f, -0.15f);
	glVertex2f(1.0f, -0.15f);

	glEnd();
}
