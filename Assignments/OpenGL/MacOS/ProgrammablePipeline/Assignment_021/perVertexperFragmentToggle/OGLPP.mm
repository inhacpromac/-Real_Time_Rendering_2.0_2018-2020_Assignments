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
bool isPerVertexLightEnabled = false;
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
	[window setTitle:@"macOS Window:perVertex perFragment Toggle"];
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
	
	GLuint gVertexShaderObjectPerVertex;
    GLuint gFragmentShaderObjectPerVertex;
    GLuint gVertexShaderObjectPerFragment;
    GLuint gFragmentShaderObjectPerFragment;
	GLuint gShaderProgramObjectPerVertex;
    GLuint gShaderProgramObjectPerFragment;
	
    GLuint mUniformPerVertex;
    GLuint vUniformPerVertex;
    GLuint pUniformPerVertex;

    GLuint laUniformPerVertex;
    GLuint ldUniformPerVertex;
    GLuint lsUniformPerVertex;
    GLuint kaUniformPerVertex;
    GLuint kdUniformPerVertex;
    GLuint ksUniformPerVertex;
    GLuint materialShininessUniformPerVertex;
    GLuint lightPositionUniformPerVertex;
    GLuint lightingEnabledUniformPerVertex;

    GLuint mUniformPerFragment;
    GLuint vUniformPerFragment;
    GLuint pUniformPerFragment;
    
    GLuint laUniformPerFragment;
    GLuint ldUniformPerFragment;
    GLuint lsUniformPerFragment;
    GLuint kaUniformPerFragment;
    GLuint kdUniformPerFragment;
    GLuint ksUniformPerFragment;
    GLuint materialShininessUniformPerFragment;
    GLuint lightPositionUniformPerFragment;
    GLuint lightingEnabledUniformPerFragment;
	
	vmath::mat4 perspectiveProjectionMatrix;
}

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 100.0f,100.0f,100.0f,1.0f };

GLfloat materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
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
    //xx//xx//xx//PerVertexInitilization//xx//xx//xx//
	//VERTEX SHADER
	//create shader
	gVertexShaderObjectPerVertex = glCreateShader(GL_VERTEX_SHADER);
	
	//provide source code to shader
	const GLchar *vertexShaderSourceCodePerVertex =
	"#version 410" \
	"\n " \
	"in vec4 vPosition;" \
    "in vec3 vNormal;" \
    "uniform mat4 u_m_matrix;" \
    "uniform mat4 u_v_matrix;" \
    "uniform mat4 u_p_matrix;" \
    "uniform int u_lkeypressed;" \
    "uniform vec3 u_la;" \
    "uniform vec3 u_ld;" \
    "uniform vec3 u_ls;" \
    "uniform vec3 u_ka;" \
    "uniform vec3 u_kd;" \
    "uniform vec3 u_ks;" \
    "uniform vec4 u_light_position;" \
    "uniform float u_material_shininess;" \
    "out vec3 phong_ads_light;" \
    "void main(void)" \
    "{" \
    "   if(u_lkeypressed == 1)" \
    "   {" \
    "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
    "       vec3 tNorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" \
    "       vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));" \
    "       float tn_dot_ld = max(dot(light_direction, tNorm), 0.0);" \
    "       vec3 reflection_vector = reflect(-light_direction, tNorm);" \
    "       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
    "       vec3 ambient = u_la * u_ka;" \
    "       vec3 diffuse = u_ld * u_kd * tn_dot_ld;" \
    "       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" \
    "       phong_ads_light = ambient + diffuse + specular;"
    "   }" \
    "   else" \
    "   {" \
    "       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
    "   }" \
    "   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
    "}";

	glShaderSource(gVertexShaderObjectPerVertex, 1, (const GLchar **)&vertexShaderSourceCodePerVertex, NULL);
	
	//compile shader
	glCompileShader(gVertexShaderObjectPerVertex);
	GLint iInfoLogLength = 0;
	GLint iShaderCompiledStatus = 0;
	char *szInfoLog = NULL;
	glGetShaderiv(gVertexShaderObjectPerVertex, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if(iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gVertexShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if(iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if(szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gVertexShaderObjectPerVertex, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Vertex Shader PerVertex Compilation Log : %s\n", szInfoLog);
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
	gFragmentShaderObjectPerVertex = glCreateShader(GL_FRAGMENT_SHADER);
	
	//provide source code to shader
	const GLchar *fragmentShaderSourceCodePerVertex =
	"#version 410" \
	"\n" \
	"in vec3 phong_ads_light;" \
    "out vec4 FragColor;" \
    "uniform int u_lkeypressed;" \
    "void main(void)" \
    "{" \
    "   if(u_lkeypressed == 1)" \
    "   {" \
    "       FragColor = vec4(phong_ads_light, 1.0);" \
    "   }" \
    "   else" \
    "   {" \
    "       FragColor = vec4(1.0, 1.0, 1.0, 1.0);" \
    "   }" \
    "}";

	
	glShaderSource(gFragmentShaderObjectPerVertex, 1, (const GLchar **)&fragmentShaderSourceCodePerVertex, NULL);
	
	// compile shader
	glCompileShader(gFragmentShaderObjectPerVertex);
	glGetShaderiv(gFragmentShaderObjectPerVertex, GL_COMPILE_STATUS, &iShaderCompiledStatus);
	if (iShaderCompiledStatus == GL_FALSE)
	{
		glGetShaderiv(gFragmentShaderObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(gFragmentShaderObjectPerVertex, iInfoLogLength, &written, szInfoLog);
				fprintf(gpFile, "Fragment Shader PerVertex Compilation Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	//SHADER PROGRAM
	//create
	gShaderProgramObjectPerVertex = glCreateProgram();
	
	//attach vertex shader to shader program
	glAttachShader(gShaderProgramObjectPerVertex, gVertexShaderObjectPerVertex);
	
	// attach fragment shader to shader program
	glAttachShader(gShaderProgramObjectPerVertex, gFragmentShaderObjectPerVertex);
	
	//prelink binding
	glBindAttribLocation(gShaderProgramObjectPerVertex, AMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObjectPerVertex, AMC_ATTRIBUTE_NORMAL, "vNormal");
	
	//link shader
	glLinkProgram(gShaderProgramObjectPerVertex);
	GLint iShaderProgramLinkStatus = 0;
	glGetProgramiv(gShaderProgramObjectPerVertex,  GL_LINK_STATUS, &iShaderProgramLinkStatus);
	if (iShaderProgramLinkStatus == GL_FALSE)
	{
		glGetProgramiv(gShaderProgramObjectPerVertex, GL_INFO_LOG_LENGTH, &iInfoLogLength);
		if (iInfoLogLength > 0)
		{
			szInfoLog = (char *)malloc(iInfoLogLength);
			if (szInfoLog != NULL)
			{
				GLsizei written ;
				glGetProgramInfoLog(gShaderProgramObjectPerVertex, iInfoLogLength, &written, szInfoLog);
				fprintf (gpFile, "Shader Program PerVertex Link Log : %s\n", szInfoLog);
				free(szInfoLog);
				[self release];
				[NSApp terminate:self];
			}
		}
	}
	
	//get MVPuniform location
	lightingEnabledUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_lkeypressed");
    mUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_m_matrix");
    vUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_v_matrix");
    pUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_p_matrix");
    laUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_la");
    ldUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ld");
    lsUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ls");
    kaUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ka");
    kdUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_kd");
    ksUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_ks");
    materialShininessUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_material_shininess");
    lightPositionUniformPerVertex = glGetUniformLocation(gShaderProgramObjectPerVertex, "u_light_position");
    
    //xx//xx//xx//PerFragmentInitilization//xx//xx//xx//
    //VERTEX SHADER
    //create shader
    gVertexShaderObjectPerFragment = glCreateShader(GL_VERTEX_SHADER);
    
    //provide source code to shader
    const GLchar *vertexShaderSourceCodePerFragment =
    "#version 410" \
    "\n " \
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

    glShaderSource(gVertexShaderObjectPerFragment, 1, (const GLchar **)&vertexShaderSourceCodePerFragment, NULL);
    
    //compile shader
    glCompileShader(gVertexShaderObjectPerFragment);
    iInfoLogLength = 0;
    iShaderCompiledStatus = 0;
    szInfoLog = NULL;
    glGetShaderiv(gVertexShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if(iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gVertexShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if(iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if(szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gVertexShaderObjectPerFragment, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Vertex Shader PerFragment Compilation Log : %s\n", szInfoLog);
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
    gFragmentShaderObjectPerFragment = glCreateShader(GL_FRAGMENT_SHADER);
    
    //provide source code to shader
    const GLchar *fragmentShaderSourceCodePerFragment =
    "#version 410" \
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
    "    FragColor = vec4(phong_ads_light, 1.0);" \
    "}";


    
    glShaderSource(gFragmentShaderObjectPerFragment, 1, (const GLchar **)&fragmentShaderSourceCodePerFragment, NULL);
    
    // compile shader
    glCompileShader(gFragmentShaderObjectPerFragment);
    glGetShaderiv(gFragmentShaderObjectPerFragment, GL_COMPILE_STATUS, &iShaderCompiledStatus);
    if (iShaderCompiledStatus == GL_FALSE)
    {
        glGetShaderiv(gFragmentShaderObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written;
                glGetShaderInfoLog(gFragmentShaderObjectPerFragment, iInfoLogLength, &written, szInfoLog);
                fprintf(gpFile, "Fragment Shader PerFragment Compilation Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    //SHADER PROGRAM
    //create
    gShaderProgramObjectPerFragment = glCreateProgram();
    
    //attach vertex shader to shader program
    glAttachShader(gShaderProgramObjectPerFragment, gVertexShaderObjectPerFragment);
    
    // attach fragment shader to shader program
    glAttachShader(gShaderProgramObjectPerFragment, gFragmentShaderObjectPerFragment);
    
    //prelink binding
    glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_POSITION, "vPosition");
    glBindAttribLocation(gShaderProgramObjectPerFragment, AMC_ATTRIBUTE_NORMAL, "vNormal");
    
    //link shader
    glLinkProgram(gShaderProgramObjectPerFragment);
    iShaderProgramLinkStatus = 0;
    glGetProgramiv(gShaderProgramObjectPerFragment,  GL_LINK_STATUS, &iShaderProgramLinkStatus);
    if (iShaderProgramLinkStatus == GL_FALSE)
    {
        glGetProgramiv(gShaderProgramObjectPerFragment, GL_INFO_LOG_LENGTH, &iInfoLogLength);
        if (iInfoLogLength > 0)
        {
            szInfoLog = (char *)malloc(iInfoLogLength);
            if (szInfoLog != NULL)
            {
                GLsizei written ;
                glGetProgramInfoLog(gShaderProgramObjectPerFragment, iInfoLogLength, &written, szInfoLog);
                fprintf (gpFile, "Shader Program PerFragment Link Log : %s\n", szInfoLog);
                free(szInfoLog);
                [self release];
                [NSApp terminate:self];
            }
        }
    }
    
    //get MVPuniform location
    lightingEnabledUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_lkeypressed");
    mUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_m_matrix");
    vUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_v_matrix");
    pUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_p_matrix");
    laUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_la");
    ldUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ld");
    lsUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ls");
    kaUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ka");
    kdUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_kd");
    ksUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_ks");
    materialShininessUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_material_shininess");
    lightPositionUniformPerFragment = glGetUniformLocation(gShaderProgramObjectPerFragment, "u_light_position");
	
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
	
	if (isPerVertexLightEnabled)
    {
        glUseProgram(gShaderProgramObjectPerVertex);

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
            glUniform1i(lightingEnabledUniformPerVertex, 1);
            glUniform3fv(laUniformPerVertex, 1, lightAmbient);
            glUniform3fv(ldUniformPerVertex, 1, lightDiffuse);
            glUniform3fv(lsUniformPerVertex, 1, lightSpecular);
            glUniform3fv(kaUniformPerVertex, 1, materialAmbient);
            glUniform3fv(kdUniformPerVertex, 1, materialDiffuse);
            glUniform3fv(ksUniformPerVertex, 1, materialSpecular);
            glUniform1f(materialShininessUniformPerVertex, materialShininess);
            glUniform4fv(lightPositionUniformPerVertex, 1, lightPosition);
        }
        else
        {
            glUniform1i(lightingEnabledUniformPerVertex, 0);
        }

        //Send necessary Matrix to Shader in respective Uniform
        glUniformMatrix4fv(mUniformPerVertex, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(vUniformPerVertex, 1, GL_FALSE, ViewMatrix);
        glUniformMatrix4fv(pUniformPerVertex, 1, GL_FALSE, projectionMatrix);

        //draw
        sphereDraw();

        glUseProgram(0);
    }
    else
    {
        glUseProgram(gShaderProgramObjectPerFragment);

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
            glUniform1i(lightingEnabledUniformPerFragment, 1);
            glUniform3fv(laUniformPerFragment, 1, lightAmbient);
            glUniform3fv(ldUniformPerFragment, 1, lightDiffuse);
            glUniform3fv(lsUniformPerFragment, 1, lightSpecular);
            glUniform3fv(kaUniformPerFragment, 1, materialAmbient);
            glUniform3fv(kdUniformPerFragment, 1, materialDiffuse);
            glUniform3fv(ksUniformPerFragment, 1, materialSpecular);
            glUniform1f(materialShininessUniformPerFragment, materialShininess);
            glUniform4fv(lightPositionUniformPerFragment, 1, lightPosition);
        }
        else
        {
            glUniform1i(lightingEnabledUniformPerFragment, 0);
        }

        //Send necessary Matrix to Shader in respective Uniform
        glUniformMatrix4fv(mUniformPerFragment, 1, GL_FALSE, modelMatrix);
        glUniformMatrix4fv(vUniformPerFragment, 1, GL_FALSE, ViewMatrix);
        glUniformMatrix4fv(pUniformPerFragment, 1, GL_FALSE, projectionMatrix);

        //draw
        sphereDraw();

        glUseProgram(0);
    }
	
	
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
        case 'T':
            isPerVertexLightEnabled = true;
            break;
        case 't':
            isPerVertexLightEnabled = false;
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
	glDetachShader(gShaderProgramObjectPerVertex, gVertexShaderObjectPerVertex);
	
	//detach fragment shader from shader program object
	glDetachShader(gShaderProgramObjectPerVertex, gFragmentShaderObjectPerVertex);
	
	//delete vertex shader object
	glDeleteShader(gVertexShaderObjectPerVertex);
	gVertexShaderObjectPerVertex = 0;
	
	//delete fragment shader object
	glDeleteShader(gFragmentShaderObjectPerVertex);
	gFragmentShaderObjectPerVertex = 0;
	
	//delete shader program object
	glDeleteProgram(gShaderProgramObjectPerVertex);
	gShaderProgramObjectPerVertex = 0;
    
    //detach vertex shader from shader program object
    glDetachShader(gShaderProgramObjectPerFragment, gVertexShaderObjectPerFragment);
    
    //detach fragment shader from shader program object
    glDetachShader(gShaderProgramObjectPerFragment, gFragmentShaderObjectPerFragment);
    
    //delete vertex shader object
    glDeleteShader(gVertexShaderObjectPerFragment);
    gVertexShaderObjectPerFragment = 0;
    
    //delete fragment shader object
    glDeleteShader(gFragmentShaderObjectPerFragment);
    gFragmentShaderObjectPerFragment = 0;
    
    //delete shader program object
    glDeleteProgram(gShaderProgramObjectPerFragment);
    gShaderProgramObjectPerFragment = 0;
	
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
