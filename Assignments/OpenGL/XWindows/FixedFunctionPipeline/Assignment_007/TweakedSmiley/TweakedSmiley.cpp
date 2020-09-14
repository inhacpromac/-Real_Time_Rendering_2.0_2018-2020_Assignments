//Assignment_007
//TweakedSmiley

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
#include <SOIL/SOIL.h>

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
GLuint Texture_Smiley;
int passedDigit = 0;

GLXContext gGLXContext;

void resize(int, int);
bool loadTextures(GLuint *texture, const char *path);

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
						case '0':
							passedDigit = 0;
							break;
						case '1':
							passedDigit = 1;
							break;
						case '2':
							passedDigit = 2;
							break;
						case '3':
							passedDigit = 3;
							break;
						case '4':
							passedDigit = 4;
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

	glEnable(GL_TEXTURE_2D);
    loadTextures(&Texture_Smiley, "./Smiley.bmp");

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

bool loadTextures(GLuint *texture, const char *path)
{
	bool bResult = false;
    int width = 0;
    int height = 0;

    unsigned char *imageData = NULL;
	imageData = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);

    if (imageData == NULL)
    {
    	bResult = false;
    	return bResult;
    }
    else
	{
		bResult = true;
		glGenTextures(1, texture);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
		glBindTexture(GL_TEXTURE_2D, *texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        SOIL_free_image_data(imageData);
	}

	return bResult;
}

void display(void)
{
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -5.0f);
	
	if (passedDigit == 0)         //White
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glBegin(GL_QUADS);

		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, 0.0f);


		glEnd();
	}
	else if (passedDigit == 1)    //BottomLeftQuater
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(0.5f, 0.5f);
        glVertex3f(1.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 0.5f);
        glVertex3f(-1.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);

        glTexCoord2f(0.5f, 1.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	else if (passedDigit == 2)    //FullTexture
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(1.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);

        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	else if (passedDigit == 4)    //Centered
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, 1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(-1.0f, -1.0f, 0.0f);

		glTexCoord2f(0.5f, 0.5f);
		glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
	}
	else if (passedDigit = 3)    //Tilled
	{
		glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
		glBegin(GL_QUADS);

		glTexCoord2f(2.0f, 0.0f);
        glVertex3f(1.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(-1.0f, 1.0f, 0.0f);

        glTexCoord2f(0.0f, 2.0f);
        glVertex3f(-1.0f, -1.0f, 0.0f);

        glTexCoord2f(2.0f, 2.0f);
        glVertex3f(1.0f, -1.0f, 0.0f);

		glEnd();
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
	GLXContext currentGLXContext = glXGetCurrentContext();
	if(currentGLXContext != NULL && currentGLXContext == gGLXContext)
	{
		glXMakeCurrent(gpDisplay, 0, 0);
	}
	if(gGLXContext)
	{
		glXDestroyContext(gpDisplay, gGLXContext);
	}
	if (Texture_Smiley)
	{
		glDeleteTextures(1, &Texture_Smiley);
		Texture_Smiley = 0;
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
