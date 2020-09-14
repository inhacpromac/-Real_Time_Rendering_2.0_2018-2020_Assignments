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
	[window setTitle:@"macOS Window:Three Rotating Lights On Sphere"];
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
	
    GLuint mUniform;
    GLuint vUniform;
    GLuint pUniform;
    GLuint laZeroUniform;
    GLuint ldZeroUniform;
    GLuint lsZeroUniform;
    GLuint laOneUniform;
    GLuint ldOneUniform;
    GLuint lsOneUniform;
    GLuint laTwoUniform;
    GLuint ldTwoUniform;
    GLuint lsTwoUniform;
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;
    GLuint lightZeroPositionUniform;
    GLuint lightOnePositionUniform;
    GLuint lightTwoPositionUniform;
    GLuint lightingEnabledUniform;

	
	vmath::mat4 perspectiveProjectionMatrix;
}

GLfloat angleOfLightZero = 0.0f;
GLfloat angleOfLightOne = 0.0f;
GLfloat angleOfLightTwo = 0.0f;
GLfloat speed = 0.1f;

GLfloat lightZeroAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroDiffuse[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroSpecular[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroPosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat lightOneAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightOneDiffuse[] = { 0.0f, 1.0f, 0.0f, 0.0f };
GLfloat lightOneSpecular[] = { 0.0f, 1.0f, 0.0f, 0.0f };
GLfloat lightOnePosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat lightTwoAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightTwoDiffuse[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat lightTwoSpecular[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat lightTwoPosition[] = { 0.0f, 0.0f, 0.0f, 0.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat materialShininess = 128.0f;    //50.0f

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
    "uniform mat4 u_m_matrix;" \
    "uniform mat4 u_v_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform int u_lkeypressed;" \
    "uniform vec4 u_light_position_zero;" \
    "uniform vec4 u_light_position_one;" \
    "uniform vec4 u_light_position_two;" \
    "out vec3 tNorm;" \
    "out vec3 light_direction_zero;" \
    "out vec3 light_direction_one;" \
    "out vec3 light_direction_two;" \
    "out vec3 viewer_vector;" \
    "void main(void)" \
    "{" \
    "   if(u_lkeypressed == 1)" \
    "   {" \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
    "       tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" \
    "       light_direction_zero = vec3(u_light_position_zero - eye_coordinates);" \
    "       light_direction_one = vec3(u_light_position_one - eye_coordinates);" \
    "       light_direction_two = vec3(u_light_position_two - eye_coordinates);" \
    "       viewer_vector = -eye_coordinates.xyz;" \
    "   }" \
    "   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
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
	"in vec3 tNorm;" \
        "in vec3 light_direction_zero;" \
        "in vec3 light_direction_one;" \
        "in vec3 light_direction_two;" \
        "in vec3 viewer_vector;" \
        "uniform int u_lkeypressed;" \
        "uniform vec3 u_la_zero;" \
        "uniform vec3 u_ld_zero;" \
        "uniform vec3 u_ls_zero;" \
        "uniform vec3 u_la_one;" \
        "uniform vec3 u_ld_one;" \
        "uniform vec3 u_ls_one;" \
        "uniform vec3 u_la_two;" \
        "uniform vec3 u_ld_two;" \
        "uniform vec3 u_ls_two;" \
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
        "       vec3 normalized_viewer_vector = normalize(viewer_vector);" \
        "        \n" \
        "       vec3 normalized_light_direction_zero = normalize(light_direction_zero);" \
        "       float tn_dot_ld_zero = max(dot(normalized_light_direction_zero, normalized_tNorm), 0.0);" \
        "       vec3 ambient_zero = u_la_zero * u_ka;" \
        "       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" \
        "       vec3 reflection_vector_zero = reflect(-normalized_light_direction_zero, normalized_tNorm);" \
        "       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, normalized_light_direction_zero), 0.0), u_material_shininess);" \
        "        \n" \
        "       vec3 normalized_light_direction_one = normalize(light_direction_one);" \
        "       float tn_dot_ld_one = max(dot(normalized_light_direction_one, normalized_tNorm), 0.0);" \
        "       vec3 ambient_one = u_la_one * u_ka;" \
        "       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" \
        "       vec3 reflection_vector_one = reflect(-normalized_light_direction_one, normalized_tNorm);" \
        "       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, normalized_light_direction_one), 0.0), u_material_shininess);" \
        "        \n" \
        "       vec3 normalized_light_direction_two = normalize(light_direction_two);" \
        "       float tn_dot_ld_two = max(dot(normalized_light_direction_two, normalized_tNorm), 0.0);" \
        "       vec3 ambient_two = u_la_two * u_ka;" \
        "       vec3 diffuse_two = u_ld_two * u_kd * tn_dot_ld_two;" \
        "       vec3 reflection_vector_two = reflect(-normalized_light_direction_two, normalized_tNorm);" \
        "       vec3 specular_two = u_ls_two * u_ks * pow(max(dot(reflection_vector_two, normalized_light_direction_two), 0.0), u_material_shininess);" \
        "        \n" \
        "       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one + ambient_two + diffuse_two + specular_two;"
        "   }" \
        "   else" \
        "   {" \
        "       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "    FragColor = vec4(phong_ads_light, 1.0);" \
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
    mUniform = glGetUniformLocation(shaderProgramObject, "u_m_matrix");
    vUniform = glGetUniformLocation(shaderProgramObject, "u_v_matrix");
    pUniform = glGetUniformLocation(shaderProgramObject, "u_p_matrix");
    laZeroUniform = glGetUniformLocation(shaderProgramObject, "u_la_zero");
    ldZeroUniform = glGetUniformLocation(shaderProgramObject, "u_ld_zero");
    lsZeroUniform = glGetUniformLocation(shaderProgramObject, "u_ls_zero");
    laOneUniform = glGetUniformLocation(shaderProgramObject, "u_la_one");
    ldOneUniform = glGetUniformLocation(shaderProgramObject, "u_ld_one");
    lsOneUniform = glGetUniformLocation(shaderProgramObject, "u_ls_one");
    laTwoUniform = glGetUniformLocation(shaderProgramObject, "u_la_two");
    ldTwoUniform = glGetUniformLocation(shaderProgramObject, "u_ld_two");
    lsTwoUniform = glGetUniformLocation(shaderProgramObject, "u_ls_two");
    kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
    kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
    ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
    lightZeroPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position_zero");
    lightOnePositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position_one");
    lightTwoPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position_two");
	
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
    vmath::mat4 modelMatrix;
    vmath::mat4 ViewMatrix;
    vmath::mat4 projectionMatrix;
    vmath::mat4 translationMatrix;

    //Initialize above changed Matrices to identity
    modelMatrix = vmath::mat4::identity();
    ViewMatrix = vmath::mat4::identity();
    projectionMatrix = vmath::mat4::identity();
    translationMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    //Do necessary Matrix Multiplication
    modelMatrix = modelMatrix * translationMatrix;
    projectionMatrix = projectionMatrix * perspectiveProjectionMatrix;

    if (gbLighting == true)
    {
        glUniform1i(lightingEnabledUniform, 1);

        glUniform3fv(laZeroUniform, 1, lightZeroAmbient);
        glUniform3fv(ldZeroUniform, 1, lightZeroDiffuse);
        glUniform3fv(lsZeroUniform, 1, lightZeroSpecular);

        glUniform3fv(laOneUniform, 1, lightOneAmbient);
        glUniform3fv(ldOneUniform, 1, lightOneDiffuse);
        glUniform3fv(lsOneUniform, 1, lightOneSpecular);

        glUniform3fv(laTwoUniform, 1, lightTwoAmbient);
        glUniform3fv(ldTwoUniform, 1, lightTwoDiffuse);
        glUniform3fv(lsTwoUniform, 1, lightTwoSpecular);

        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);

        glUniform1f(materialShininessUniform, materialShininess);

        glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition);
        glUniform4fv(lightOnePositionUniform, 1, lightOnePosition);
        glUniform4fv(lightTwoPositionUniform, 1, lightTwoPosition);
    }
    else
    {
        glUniform1i(lightingEnabledUniform, 0);
    }

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(vUniform, 1, GL_FALSE, ViewMatrix);
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, projectionMatrix);

    //draw
    sphereDraw();
	
	glUseProgram(0);
	
	
	CGLFlushDrawable((CGLContextObj)[[self openGLContext]CGLContextObj]);
	CGLUnlockContext((CGLContextObj)[[self openGLContext]CGLContextObj]);
    
    if (gbAnimation)
    {
        angleOfLightZero += speed;
        angleOfLightOne += speed;
        angleOfLightTwo += speed;

        if (angleOfLightZero >= 360.0f)
        {
            angleOfLightZero = 0.0f;
        }
        lightZeroPosition[0] = 0.0f;
        lightZeroPosition[1] = sinf(angleOfLightZero) * 100.0f;
        lightZeroPosition[2] = cosf(angleOfLightZero) * 100.0f - 5.0f;

        if (angleOfLightOne >= 360.0f)
        {
            angleOfLightOne = 0.0f;
        }
        lightOnePosition[0] = sinf(angleOfLightOne) * 100.0f;
        lightOnePosition[1] = 0.0f;
        lightOnePosition[2] = cosf(angleOfLightOne) * 100.0f - 5.0f;

        if (angleOfLightTwo >= 360.0f)
        {
            angleOfLightTwo = 0.0f;
        }
        lightTwoPosition[0] = cosf(angleOfLightTwo) * 100.0f;
        lightTwoPosition[1] = sinf(angleOfLightTwo) * 100.0f;
        lightTwoPosition[2] = -5.0f;
    }
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
