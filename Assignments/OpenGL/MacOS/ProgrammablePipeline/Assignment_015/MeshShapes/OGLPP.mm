//headers
#define GL_SILENCE_DEPRECATION
#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

#import <QuartzCore/CVDisplayLink.h>

#import <OpenGL/gl3.h>
#import <OpenGL/gl3ext.h>

#import "vmath.h"

enum
{
	AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0,
};

//'C'style global function declarations
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,
								const CVTimeStamp *,
								const CVTimeStamp *,
								CVOptionFlags,
								CVOptionFlags *,
								void *);

//global variables
FILE *gpFile=NULL;

//interface declarations
@interface AppDelegate : NSObject <NSApplicationDelegate, NSWindowDelegate>
@end

@interface GLView : NSOpenGLView
@end

// Entry-point function
int main(int argc, const char * argv[]) 
{
	// code
	NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc]init];
	
	NSApp = [NSApplication sharedApplication];
	
	[NSApp setDelegate:[[AppDelegate alloc]init]];
	
	[NSApp run];
	
	[pPool release];
	
	return(0);
}

// interface implementations
@implementation AppDelegate
{
@private
	NSWindow *window;
	GLView *glView;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
	//code
	//log file
	NSBundle *mainBundle=[NSBundle mainBundle];
	NSString *appDirName=[mainBundle bundlePath];
	NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
	NSString *logFileNameWithPath=[NSString stringWithFormat:@"%@/Log.txt", parentDirPath];
	const char *pszLogFileNameWithPath=[logFileNameWithPath cStringUsingEncoding:NSASCIIStringEncoding];
	gpFile=fopen(pszLogFileNameWithPath,"w");
	if(gpFile==NULL)
	{
		printf("Can Not Create Log File.\nExittig ... \n");
		[self release];
		[NSApp terminate:self];
	}
	fprintf(gpFile, "Program Is Started Successfully\n");
	
	//Window
	NSRect win_rect;
	win_rect=NSMakeRect(0.0,0.0,800.0,600.0);
	
	// create simple window
	window=[[NSWindow alloc] initWithContentRect:win_rect
										styleMask:NSWindowStyleMaskTitled | 
										NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | 
										NSWindowStyleMaskResizable 
										backing:NSBackingStoreBuffered
										defer:NO];
	[window setTitle:@"macOS Window:MeshShapes"];
	[window center];

	glView=[[GLView alloc]initWithFrame:win_rect];
	
	[window setContentView:glView];
	[window setDelegate:self];
	[window makeKeyAndOrderFront:self];
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
	//code
	fprintf(gpFile, "Program is Terminated Successfully\n");
	
	if(gpFile) 
	{
		fclose(gpFile);
		gpFile=NULL;
	}
}

- (void)windowWillClose:(NSNotification *)notification
{
	//code
	[NSApp terminate:self];
}

- (void)dealloc
{
	//code
	[glView release];
	
	[window release];
	
	[super dealloc];
}
@end

@implementation GLView
{
@private
	CVDisplayLinkRef displayLink ;
	
	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;
	GLuint shaderProgramObject;
	
    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_color;

    GLuint vbo_point_element;
    int giPointElements;

    GLuint vbo_lines_element;
    int giLinesElements;

    GLuint vbo_square_element;
    int giSquareElements;

    GLuint vbo_diagonal_square_element;
    int giDigonalSquareElements;

    GLuint vbo_fan_element;
    int giFanElements;

    GLuint vao_color_square;
    GLuint vbo_color_square;
    GLuint vbo_color_square_element;
    int giColorSquareElements;
	GLuint mvpUniform;
	
	vmath::mat4 perspectiveProjectionMatrix;
}

-(id)initWithFrame:(NSRect)frame;
{
	//code
	self=[super initWithFrame:frame];
	
	if(self)
	{
		NSOpenGLPixelFormatAttribute attrs[]=
		{
			//Must specify the 4.1 Core Profile to use OpenGL 4.1
			NSOpenGLPFAOpenGLProfile,
			NSOpenGLProfileVersion4_1Core,
			//Specify the displayID to associate the GLcontext with (main display for now)
			NSOpenGLPFAScreenMask,
			CGDisplayIDToOpenGLDisplayMask(kCGDirectMainDisplay),
			NSOpenGLPFANoRecovery,
			NSOpenGLPFAAccelerated,
			NSOpenGLPFAColorSize,24,
			NSOpenGLPFADepthSize,24,
			NSOpenGLPFAAlphaSize,8,
			NSOpenGLPFADoubleBuffer,
			0};//last 0 is must...
			
			NSOpenGLPixelFormat *pixelFormat= [[[NSOpenGLPixelFormat alloc] 
												initWithAttributes:attrs]autorelease];
			if(pixelFormat==nil)
			{
				fprintf(gpFile, "No Valid OpenGL Pixel Format is Available...Exitting ...");
				[self release];
				[NSApp terminate:self];
			}
			
			NSOpenGLContext *glContext=[[[NSOpenGLContext alloc] 
										initWithFormat:pixelFormat shareContext:nil]autorelease];
			
			[self setPixelFormat:pixelFormat];
			
			[self setOpenGLContext:glContext];
			//It automatically releases the older context, if present, and sets the newer one...
	}
	return(self);
}

-(CVReturn)getFrameForTime:(const CVTimeStamp *)pOutputTime
{
	//code
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc]init];
	[self drawView];
	[pool release];
	return(kCVReturnSuccess);
}

-(void)prepareOpenGL
{
	//code
	//OpenGL Info
    giPointElements = 0;
    giLinesElements = 0;
    giSquareElements = 0;
    giDigonalSquareElements = 0;
    giFanElements = 0;
    giColorSquareElements = 0;
    
	fprintf(gpFile, "OpenGL Version : %s\n",glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	[[self openGLContext]makeCurrentContext];
	
	GLint swapInt=1;
	[[self openGLContext]setValues:&swapInt forParameter:/*NSOpenGLCPSwapInterval*/NSOpenGLContextParameterSwapInterval];
	
	//VERTEX SHADER
	//create shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	
	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
	"#version 410" \
	"\n " \
	"in vec4 vPosition;" \
    "in vec4 vColor;" \
    "uniform mat4 u_mvp_matrix;" \
    "out vec4 out_color;" \
    "void main(void)" \
    "{" \
    "   gl_Position = u_mvp_matrix * vPosition;" \
    "    out_color = vColor;" \
    "}";
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
	
	//compile shader
	glCompileShader(vertexShaderObject);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if(szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release]; [NSApp terminate:self];
			}
		}
	}
	
	//FRAGMENT SHADER
	//re-initialize
	iInfoLogLength = 0;
	iShaderCompiledStatus = 0;
	szInfoLog = NULL;
	
	//create shader
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	
	//provide source code to shader
	const GLchar *fragmentShaderSourceCode =
	"#version 410" \
	"\n" \
	"in vec4 out_color;" \
    "out vec4 FragColor;" \
    "void main(void)" \
    "{" \
    "    FragColor = out_color;" \
    "}";
	
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fragmentShaderSourceCode, NULL);
	
	// compile shader
	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	//SHADER PROGRAM
	//create
	shaderProgramObject = glCreateProgram();
	
	//attach vertex shader to shader program
	glAttachShader(shaderProgramObject, vertexShaderObject);
	
	// attach fragment shader to shader program
	glAttachShader(shaderProgramObject, fragmentShaderObject);
	
	//prelink binding
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
	
	//link shader
	glLinkProgram(shaderProgramObject);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(shaderProgramObject,  GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written ;
				glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
				fprintf (gpFile, "Shader Program Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	//get MVPuniform location
	mvpUniform = glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
	
	//vertices, colors, shader attribs, vbo, vao initializations
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
	glEnable(GL_TEXTURE_2D);
	
	//W'll always cull back faces for better performance
	//glEnable(GL_CULL_FACE);
	
	//set background color
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	//set projection matrix to identity matrix
	perspectiveProjectionMatrix = vmath::mat4::identity();
	
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
	CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
	CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
	CVDisplayLinkStart(displayLink);
}

-(void)reshape
{
	//code
	CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
	
	NSRect rect=[self bounds];
	
	GLfloat width=rect.size.width;
	GLfloat height=rect.size.height;
	
	if(height==0)
		height=1;
		
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	
	perspectiveProjectionMatrix = vmath::perspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);

	
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect
{
	// code
	[self drawView];
}

- (void)drawView
{
	[[self openGLContext]makeCurrentContext];
	
	CGLLockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//start using OpenGL program object
	glUseProgram(shaderProgramObject);
	
    //Declaration of Matrices
    vmath::mat4 modelViewMatrix;
    vmath::mat4 modelViewProjectionMatrix;

    //Initialize above Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(-3.0f, 1.0f, -8.0f);

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
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(0.0f, 1.0f, -8.0f);

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
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(3.0f, 1.0f, -8.0f);

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
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(-3.0f, -1.0f, -8.0f);

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
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(0.0f, -1.0f, -8.0f);

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
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(2.5f, -1.0f, -8.0f);

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
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    modelViewMatrix = vmath::translate(3.0f, -1.0f, -8.0f);

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
	
	
	CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
	CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
}

-(BOOL)acceptsFirstResponder
{
	// code
	[[self window]makeFirstResponder:self];
	return(YES);
}


-(void)keyDown:(NSEvent *)theEvent
{
	//code
	int key=(int)[[theEvent characters]characterAtIndex:0];
	switch(key)
	{
		case 27: //Esc key
			[ self release];
			[NSApp terminate:self];
			break;
		case 'F':
		case 'f':
			[[self window]toggleFullScreen:self]; // repainting occurs automatically
			break;
		default:
			break;
	}
}

-(void)mouseDown:(NSEvent *)theEvent
{
	//code
}

-(void)mouseDragged:(NSEvent *)theEvent
{
	//code
}

-(void)rightMouseDown:(NSEvent *)theEvent
{
	//code
}

- (void) dealloc
{
	// code
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
	
	//detach vertex shader from shader program object
	glDetachShader(shaderProgramObject, vertexShaderObject);
	
	//detach fragment shader from shader program object
	glDetachShader(shaderProgramObject, fragmentShaderObject);
	
	//delete vertex shader object
	glDeleteShader(vertexShaderObject);
	vertexShaderObject = 0;
	
	//delete fragment shader object
	glDeleteShader(fragmentShaderObject);
	fragmentShaderObject = 0;
	
	//delete shader program object
	glDeleteProgram(shaderProgramObject);
	shaderProgramObject = 0;
	
	CVDisplayLinkStop(displayLink);
	CVDisplayLinkRelease(displayLink);
	
	[super dealloc];
}

@end

CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink,
								const CVTimeStamp *pNoW,
								const CVTimeStamp *pOutputTime,
								CVOptionFlags flagsIn ,
								CVOptionFlags *pFlagsOut,
								void *pDisplayLinkContext)
{
	CVReturn result=[(GLView *)pDisplayLinkContext getFrameForTime:pOutputTime];
	return(result);
}
