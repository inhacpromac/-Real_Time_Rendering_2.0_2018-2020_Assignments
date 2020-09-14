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
	[window setTitle:@"macOS Window:TweakedSmiley"];
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
    GLuint vbo_texture;
	GLuint mvpUniform;
	GLuint Texture_Smiley;
	vmath::mat4 projectionMatrix;
	
	GLuint texture_sampler_uniform;
    
    GLuint passedDigit;
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
												initWithAttributes:attrs] autorelease];
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
    passedDigit = 0;
	//OpenGL Info
	fprintf(gpFile, "OpenGL Version : %s\n",glGetString(GL_VERSION));
	fprintf(gpFile, "GLSL Version : %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	[[self openGLContext]makeCurrentContext];
	
	GLint swaplnt=1;
	[[self openGLContext]setValues:&swaplnt forParameter:/*NSOpenGLCPSwapInterval*/NSOpenGLContextParameterSwapInterval];
	
	//VERTEX SHADER
	//create shader
	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	
	//provide source code to shader
	const GLchar *vertexShaderSourceCode =
	"#version 410" \
	"\n " \
	"in vec4 vPosition ;" \
	"in vec2 vTexture0_Coord;" \
	"out vec2 out_texture0_coord;" \
	"uniform mat4 u_mvp_matrix;" \
	"void main(void)" \
	"{" \
	"	gl_Position = u_mvp_matrix * vPosition;" \
	"	out_texture0_coord=vTexture0_Coord;" \
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
	"in vec2 out_texture0_coord;" \
	"uniform sampler2D u_texture0_sampler;" \
	"out vec4 FragColor;" \
	"void main(void)" \
	"{" \
	"	vec3 tex=vec3(texture(u_texture0_sampler, out_texture0_coord));" \
	"	FragColor=vec4(tex, 1.0f);" \
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
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexture0_Coord");
	
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
	//get texture sampler uniform location
	texture_sampler_uniform=glGetUniformLocation(shaderProgramObject,"u_texture0_sampler");
	
	//load textures
	Texture_Smiley=[self loadTextureFromBMPFile:"Smiley.bmp"];
	
	//vertices, colors, shader attribs, vbo, vao initializations
	const GLfloat rectangleVertices[] =
    {
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f
    };

    //Create vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    //position
    glGenBuffers(1, &vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //Texture
    glGenBuffers(1, &vbo_texture);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);

    glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);

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
	projectionMatrix = vmath::mat4::identity(); 
	
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink,&MyDisplayLinkCallback,self);
	CGLContextObj cglContext=(CGLContextObj)[[self openGLContext]CGLContextObj];
	CGLPixelFormatObj cglPixelFormat=(CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink,cglContext,cglPixelFormat);
	CVDisplayLinkStart(displayLink);
}

-(GLuint)loadTextureFromBMPFile:(const char *)texFileName
{
	NSBundle *mainBundle=[NSBundle mainBundle];
	NSString *appDirName=[mainBundle bundlePath];
	NSString *parentDirPath=[appDirName stringByDeletingLastPathComponent];
	NSString *textureFileNameWithPath=[NSString stringWithFormat:@"%@/%s",parentDirPath,texFileName];
	
	NSImage *bmplmage=[[NSImage alloc] initWithContentsOfFile:textureFileNameWithPath];\
	if (!bmplmage)
	{
		NSLog(@"can't find %@", textureFileNameWithPath);
		return(0);
	}
	CGImageRef cglmage = [bmplmage CGImageForProposedRect:nil context:nil hints:nil];
	int w = (int)CGImageGetWidth(cglmage);
	int h = (int)CGImageGetHeight(cglmage);
	CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cglmage));
	void* pixels = (void *)CFDataGetBytePtr(imageData);
	
	GLuint bmpTexture;
	glGenTextures(1, &bmpTexture);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //set 1 rather than default 4 for better performance
	glBindTexture(GL_TEXTURE_2D, bmpTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,	0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	//create mipmaps for this texture for better image quality
	glGenerateMipmap(GL_TEXTURE_2D);
	
	CFRelease(imageData);
	return(bmpTexture);
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
	
	// perspective projection
	projectionMatrix=vmath::perspective(45.0f, width/height, 0.1f, 100.0f);
	
	CGLUnlockContext((CGLContextObj)[[self openGLContext] CGLContextObj]);
}

- (void)drawRect:(NSRect)dirtyRect
{
	// code
	[self drawView];
}

- (void)drawView
{
	// code
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
    modelViewMatrix = vmath::translate(0.0f, 0.0f, -6.0f);

    //Do necessary Matrix Multiplication
    modelViewProjectionMatrix = projectionMatrix * modelViewMatrix;//this was internally done by gluOrtho2D/glOrtho in FFP

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Work with texture [A][B][U]
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture_Smiley);
    glUniform1i(texture_sampler_uniform, 0);

    //Bind with vao
    glBindVertexArray(vao);
    
    //Texture applies dynamically
    GLfloat reactangleTexture[8] = { 0.0f };

    if (passedDigit == 1)
    {
        reactangleTexture[0] = 0.5f;
        reactangleTexture[1] = 0.0f;
        reactangleTexture[2] = 0.5f;
        reactangleTexture[3] = 0.5f;
        reactangleTexture[4] = 0.0f;
        reactangleTexture[5] = 0.5f;
        reactangleTexture[6] = 0.0f;
        reactangleTexture[7] = 0.0f;
    }
    else if (passedDigit == 2)
    {
        reactangleTexture[0] = 1.0f;
        reactangleTexture[1] = 0.0f;
        reactangleTexture[2] = 1.0f;
        reactangleTexture[3] = 1.0f;
        reactangleTexture[4] = 0.0f;
        reactangleTexture[5] = 1.0f;
        reactangleTexture[6] = 0.0f;
        reactangleTexture[7] = 0.0f;
    }
    else if (passedDigit == 3)
    {
        reactangleTexture[0] = 2.0f;
        reactangleTexture[1] = 0.0f;
        reactangleTexture[2] = 2.0f;
        reactangleTexture[3] = 2.0f;
        reactangleTexture[4] = 0.0f;
        reactangleTexture[5] = 2.0f;
        reactangleTexture[6] = 0.0f;
        reactangleTexture[7] = 0.0f;
    }
    else if (passedDigit == 4)
    {
        reactangleTexture[0] = 0.5f;
        reactangleTexture[1] = 0.5f;
        reactangleTexture[2] = 0.5f;
        reactangleTexture[3] = 0.5f;
        reactangleTexture[4] = 0.5f;
        reactangleTexture[5] = 0.5f;
        reactangleTexture[6] = 0.5f;
        reactangleTexture[7] = 0.5f;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
    glBufferData(GL_ARRAY_BUFFER, sizeof(reactangleTexture), reactangleTexture, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //Draw necessary Scence
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

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
        case 48:
            passedDigit = 0;
            break;
        case 49:
            passedDigit = 1;
            break;
        case 50:
            passedDigit = 2;
            break;
        case 51:
            passedDigit = 3;
            break;
        case 52:
            passedDigit = 4;
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
	if (vbo_texture)
    {
        glDeleteBuffers(1, &vbo_texture);
    }
    if (vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
    }
    if (vao)
    {
        glDeleteBuffers(1, &vao);
    }
    
    glDeleteTextures(1, &Texture_Smiley);
    Texture_Smiley = 0;
	
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
