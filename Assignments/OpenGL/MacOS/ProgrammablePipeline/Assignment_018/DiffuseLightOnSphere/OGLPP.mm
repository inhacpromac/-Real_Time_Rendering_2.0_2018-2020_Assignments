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

#import "sphere.h"

//'C'style global function declarations
CVReturn MyDisplayLinkCallback(CVDisplayLinkRef,
								const CVTimeStamp *,
								const CVTimeStamp *,
								CVOptionFlags,
								CVOptionFlags *,
								void *);

//global variables
FILE *gpFile=NULL;
bool gbAnimation = false;
bool gbLighting = false;

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
	[window setTitle:@"macOS Window:Diffuse Light On Sphere"];
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
	
    GLuint mvUniform;
    GLuint pUniform;
    GLuint ldUniform;
    GLuint kdUniform;
    GLuint lightPositionUniform;
    GLuint lightingEnabledUniform;
	
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
    "in vec3 vNormal;" \
    "uniform mat4 u_mv_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform int u_lkeypressed;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_kd;" \
    "uniform vec4 u_light_position;" \
    "out vec3 diffuse_color;" \
    "void main(void)" \
    "{" \
    "   if(u_lkeypressed == 1)" \
    "   {" \
    "       vec4 eye_coordinates = u_mv_matrix * vPosition;" \
    "        mat3 normal_matrix = mat3(transpose(inverse(u_mv_matrix)));" \
    "       vec3 tNorm = normalize(normal_matrix * vNormal);" \
    "       vec3 s = normalize(vec3(u_light_position - eye_coordinates));" \
    "       diffuse_color = u_ld * u_kd * max(dot(s, tNorm), 0.0);" \
    "   }" \
    "   gl_Position = u_p_matrix * u_mv_matrix * vPosition;" \
    "}";
    // s == light_direction
    //diffuseLight = u_ld * u_kd * max(dot(s, tNorm), 0.0)
    //max() give maximum values.
    //max() will avoid negative values by passing 0.0f if dot() gives negative value.

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
	"in vec3 diffuse_color;" \
    "out vec4 FragColor;" \
    "uniform int u_lkeypressed;" \
    "void main(void)" \
    "{" \
    "   if(u_lkeypressed == 1)" \
    "   {" \
    "       FragColor = vec4(diffuse_color, 1.0);" \
    "   }" \
    "   else" \
    "   {" \
    "       FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
    "   }" \
    "}";
    //Bad Code But done for the sake of the concept.
	
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
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
	
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
	lightingEnabledUniform = glGetUniformLocation(shaderProgramObject, "u_lkeypressed");
    mvUniform = glGetUniformLocation(shaderProgramObject, "u_mv_matrix");
    pUniform = glGetUniformLocation(shaderProgramObject, "u_p_matrix");
    ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
    kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
	
	//vertices, colors, shader attribs, vbo, vao initializations
	makeSphere(1.0, 50, 50);

	
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
    vmath::mat4 projectionMatrix;
    vmath::mat4 translationMatrix;

    //Initialize above changed Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    //projectionMatrix = vmath::mat4::identity();
    translationMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    //Do necessary Matrix Multiplication
    modelViewMatrix = modelViewMatrix * translationMatrix;

    if (gbLighting == true)
    {
        glUniform1i(lightingEnabledUniform, 1);
        glUniform3f(ldUniform, 1.0f, 1.0f, 1.0f);
        glUniform3f(kdUniform, 0.5f, 0.5f, 0.5f);

        GLfloat lightPosition[] = { 0.0f, 0.0f, 2.0f, 1.0f };
        glUniform4fv(lightPositionUniform, 1, lightPosition);
    }
    else
    {
        glUniform1i(lightingEnabledUniform, 0);
    }

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvUniform, 1, GL_FALSE, modelViewMatrix);
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    //draw
    sphereDraw();
	
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
	if (vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }

    if (vbo_normal)
    {
        glDeleteBuffers(1, &vbo_normal);
        vbo_normal = 0;
    }

    if (vbo_elements)
    {
        glDeleteBuffers(1, &vbo_elements);
        vbo_elements = 0;
    }

    if (vao)
    {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
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
