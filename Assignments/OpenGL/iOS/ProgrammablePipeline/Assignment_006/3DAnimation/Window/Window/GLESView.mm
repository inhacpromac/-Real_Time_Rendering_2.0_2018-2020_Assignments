//
//  GLESView.m
//  Window
//
//  Created by ROHAN WAGHMODE on 10/03/20.
//

#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>

#import "vmath.h"

#import "GLESView.h"

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_NORMAL,
    AMC_ATTRIBUTE_TEXCOORD0,
};

@implementation GLESView
{
    EAGLContext *eaglContext;
    
    GLuint defaultFramebuffer;
    GLuint colorRenderbuffer;
    GLuint depthRenderbuffer;
    
    id displayLink;
    NSInteger animationFramelnterval;
    BOOL isAnimating;
    
    GLuint vertexShaderObject;
    GLuint fragmentShaderObject;
    GLuint shaderProgramObject;
    
    GLuint vao_pyramid;
    GLuint vbo_position_pyramid;
    GLuint vbo_color_pyramid;
    GLuint vao_cube;
    GLuint vbo_position_cube;
    GLuint vbo_color_cube;
    GLuint mvpUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
}

- (id)initWithFrame:(CGRect)frame;
{
    // code
    self=[super initWithFrame:frame];
    
    if(self)
    {
        CAEAGLLayer *eaglLayer=(CAEAGLLayer *)super.layer;
        
        eaglLayer.opaque=YES;
        eaglLayer.drawableProperties=[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat, nil];
        
        eaglContext=[[EAGLContext alloc]initWithAPI:kEAGLRenderingAPIOpenGLES3];
        if(eaglContext==nil)
        {
            [self release];
            return(nil);
        }
        [EAGLContext setCurrentContext:eaglContext];
        
        glGenFramebuffers(1, &defaultFramebuffer);
        glGenRenderbuffers(1, &colorRenderbuffer);
        glBindFramebuffer(GL_FRAMEBUFFER,defaultFramebuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
        
        [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:eaglLayer];
        
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
        
        GLint backingWidth;
        GLint backingHeight;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

        glGenRenderbuffers(1, &depthRenderbuffer);
        glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, backingWidth, backingHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        {
            printf("Failed To Create Complete Framebuffer Object %x \n ", glCheckFramebufferStatus(GL_FRAMEBUFFER)) ;
            glDeleteFramebuffers(1, &defaultFramebuffer);
            glDeleteRenderbuffers(1, &colorRenderbuffer);
            glDeleteRenderbuffers(1, &depthRenderbuffer);
            
            return(nil);
        }
        printf("Renderer : %s | GLVersion : %s | GLSLVersion : %s \n ", glGetString(GL_RENDERER), glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));
        
        // hard coded initializations
        isAnimating=NO;
        animationFramelnterval=60; // default since iOS 8.2
        
        // *** VERTEX SHADER ***
        // create shader
        vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
        
        // provide source code to shader
        const GLchar *vertexShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "in vec4 vPosition;" \
        "in vec4 vColor;" \
        "uniform mat4 u_mvp_matrix;" \
        "out vec4 out_color;" \
        "void main(void)" \
        "{" \
        "    gl_Position = u_mvp_matrix * vPosition;" \
        "   out_color = vColor;" \
        "}";
        
        glShaderSource(vertexShaderObject, 1, (const GLchar **) &vertexShaderSourceCode, NULL);
        
        // compile shader
        glCompileShader(vertexShaderObject);
        GLint iInfoLogLength = 0;
        GLint iShaderCompiledStatus = 0;
        char *szInfoLog = NULL;
        glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &iShaderCompiledStatus);
        if (iShaderCompiledStatus == GL_FALSE)
        {
            glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if (szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetShaderInfoLog(vertexShaderObject, iInfoLogLength, &written, szInfoLog);
                    printf("Vertex Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        // *** FRAGMENT SHADER ***
        // re-initialize
        iInfoLogLength = 0;
        iShaderCompiledStatus = 0;
        szInfoLog = NULL;
        
        // create shader
        fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
        
        // provide source code to shader
        const GLchar *fragmentShaderSourceCode =
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
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
                    printf("Fragment Shader Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        // *** SHADER PROGRAM ***
        // create
        shaderProgramObject = glCreateProgram();
        
        // attach vertex shader to shader program
        glAttachShader(shaderProgramObject, vertexShaderObject);
        // attach fragment shader to shader program
        glAttachShader(shaderProgramObject, fragmentShaderObject);
        
        // pre-link binding of shader program object with vertex shader position attribute
        glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "vPosition");
        glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "vColor");
        
        // link shader
        glLinkProgram(shaderProgramObject);
        GLint iShaderProgramLinkStatus = 0;
        glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &iShaderProgramLinkStatus);
        if (iShaderProgramLinkStatus == GL_FALSE)
        {
            glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &iInfoLogLength);
            if (iInfoLogLength > 0)
            {
                szInfoLog = (char *)malloc(iInfoLogLength);
                if (szInfoLog != NULL)
                {
                    GLsizei written;
                    glGetProgramInfoLog(shaderProgramObject, iInfoLogLength, &written, szInfoLog);
                    printf("Shader Program Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
                }
            }
        }
        
        // get MVP uniform location
        mvpUniform = glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
        
        // *** vertices, colors, shader attribs, vbo, vao initializations ***
        const GLfloat pyramidVertices[] = {
            // Front face
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
    
            // Right face
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
    
            // Back face
            0.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
    
            // Left face
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f
        };
    
        const GLfloat pyramidColor[] = {
            // Front face
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    
            // Right face
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
    
            // Back face
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
    
            // Left face
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 0.0f
        };
    
        const GLfloat cubeVertices[] = {
            // Top face
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
    
            // Bottom face
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
    
            // Front face
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
    
            // Back face
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
    
            // Right face
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
    
            // Left face
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f
        };
    
        const GLfloat cubeColor[] = {
            // Top face
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
    
            // Bottom face
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
    
            // Front face
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
    
            // Back face
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
    
            // right face
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
    
            // left face
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };
    
        //Create vao_pyramid
        glGenVertexArrays(1, &vao_pyramid);
        glBindVertexArray(vao_pyramid);
        //Position
        glGenBuffers(1, &vbo_position_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_pyramid);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Color
        glGenBuffers(1, &vbo_color_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_pyramid);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidColor), pyramidColor, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
        
        //Create vao_cube
        glGenVertexArrays(1, &vao_cube);
        glBindVertexArray(vao_cube);
        //Position
        glGenBuffers(1, &vbo_position_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Color
        glGenBuffers(1, &vbo_color_cube);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_cube);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColor), cubeColor, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
        
        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        
        // depth test to do
        glDepthFunc(GL_LEQUAL);
        
        // We will always cull back faces for better performance
        //glEnable(GL_CULL_FACE);
        
        //clearcolor
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        
        //set projection matrix to identity matrix
        perspectiveProjectionMatrix = vmath::mat4::identity();
        
        //GESTURE RECOGNITION
        //Tap gesture code
        UITapGestureRecognizer *singleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onSingleTap:)];
        [singleTapGestureRecognizer setNumberOfTapsRequired:1];
        [singleTapGestureRecognizer setNumberOfTouchesRequired:1]; // touch of 1 finger
        [singleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:singleTapGestureRecognizer];
        
        UITapGestureRecognizer *doubleTapGestureRecognizer=[[UITapGestureRecognizer alloc]initWithTarget:self action:@selector(onDoubleTap:)];
        [doubleTapGestureRecognizer setNumberOfTapsRequired:2];
        [doubleTapGestureRecognizer setNumberOfTouchesRequired:1]; // touch of 1 finger
        [doubleTapGestureRecognizer setDelegate:self];
        [self addGestureRecognizer:doubleTapGestureRecognizer];
        
        // this will allow to differentiate between single tap and double tap
        [singleTapGestureRecognizer requireGestureRecognizerToFail:doubleTapGestureRecognizer];
        
        // swipe gesture
        UISwipeGestureRecognizer *swipeGestureRecognizer=[[UISwipeGestureRecognizer alloc]initWithTarget:self action:@selector(onSwipe:)];
        [self addGestureRecognizer:swipeGestureRecognizer];
        
        // long-press gesture
        UILongPressGestureRecognizer *longPressGestureRecognizer=[[UILongPressGestureRecognizer alloc]initWithTarget:self action:@selector(onLongPress:)];
        [self addGestureRecognizer:longPressGestureRecognizer];
    }
    return(self);
}

// Only override drawRect: if you perform custom drawing.
// An empty implementation adversely affects performance during animation.
- (void)drawRect:(CGRect)rect
{
    // Drawing code
}

+(Class)layerClass
{
    // code
    return([CAEAGLLayer class]);
}

-(void)drawView:(id)sender
{
    // code
    static float anglePyramid = 0.0f;
    static float angleCube = 0.0f;
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    // OpenGL Drawing
    vmath::mat4 modelViewMatrix;
    vmath::mat4 modelViewProjectionMatrix;
    vmath::mat4 translationMatrix;
    vmath::mat4 scaleMatix;
    vmath::mat4 rotationMatrix;
    

    //////PYRAMID//////

    //Initialize above Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    
    //Do necessary Trasformation
    translationMatrix = vmath::translate(-1.5f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

    //Do necessary Matrix Multiplication
    modelViewMatrix = translationMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Bind with vao_pyramid
    glBindVertexArray(vao_pyramid);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_TRIANGLES, 0, 12);

    //Unbind with vao_pyramid
    glBindVertexArray(0);

    //////CUBE//////

    //Initialize above changed Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    translationMatrix = vmath::mat4::identity();
    scaleMatix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();
    
    //Do necessary Trasformation
    scaleMatix = vmath::scale(0.75f, 0.75f, 0.75f);
    translationMatrix = vmath::translate(1.5f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(angleCube, angleCube, angleCube);

    //Do necessary Matrix Multiplication
    modelViewMatrix = modelViewMatrix * translationMatrix;
    modelViewMatrix = modelViewMatrix * scaleMatix;
    modelViewMatrix = modelViewMatrix * rotationMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Bind with vao_cube
    glBindVertexArray(vao_cube);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
    glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

    //Unbind with vao_cube
    glBindVertexArray(0);
    
    // stop using OpenGL program object
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    if (anglePyramid > 360.f)
    {
        anglePyramid = 0.0f;
    }
    if (angleCube > 360.f)
    {
        angleCube = 0.0f;
    }

    anglePyramid = anglePyramid + 1.0f;
    angleCube = angleCube + 1.0f;
}

-(void)layoutSubviews
{
    // code
    GLint width;
    GLint height;
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer*)self.layer];
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_WIDTH,&width);
    glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_HEIGHT,&height);
    
    glGenRenderbuffers(1,&depthRenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER,depthRenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,depthRenderbuffer);
    
    glViewport(0,0, width, height);
    
    //glOrtho(left,right,bottom,top,near,far);
    GLfloat fwidth = (GLfloat)width;
    GLfloat fheight = (GLfloat)height;
    
    perspectiveProjectionMatrix = vmath::perspective(45.0f, GLfloat(width) / GLfloat(height), 0.1f, 100.0f);
        
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Failed To Create Complete Framebuffer Object %x",glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    [self drawView:nil];
}
-(void)startAnimation
{
    if (!isAnimating)
    {
        displayLink=[NSClassFromString(@"CADisplayLink")displayLinkWithTarget:self selector:@selector(drawView:)];
        [displayLink setPreferredFramesPerSecond:animationFramelnterval];
        [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
        
        isAnimating=YES;
    }
}
-(void)stopAnimation
{
    if(isAnimating)
    {
        [displayLink invalidate];
        displayLink=nil;
        
        isAnimating=NO;
    }
}


// to become first responder
-(BOOL)acceptsFirstResponder
{
    // code
    return(YES);
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}

-(void)onSingleTap:(UITapGestureRecognizer *)gr
{
    
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    
}

-(void)onSwipe:(UISwipeGestureRecognizer *)gr
{
    // code
    [self release];
    exit(0);
}

-(void)onLongPress:(UILongPressGestureRecognizer *)gr
{
    
}

- (void)dealloc
{
    // code
    
    // destroy vao
    if (vbo_color_cube)
    {
        glDeleteBuffers(1, &vbo_color_cube);
    }
    if (vbo_position_cube)
    {
        glDeleteBuffers(1, &vbo_position_cube);
    }
    if (vao_cube)
    {
        glDeleteBuffers(1, &vao_cube);
    }
    if (vbo_color_pyramid)
    {
        glDeleteBuffers(1, &vbo_color_pyramid);
    }
    if (vbo_position_pyramid)
    {
        glDeleteBuffers(1, &vbo_position_pyramid);
    }
    if (vao_pyramid)
    {
        glDeleteBuffers(1, &vao_pyramid);
    }
    
    // detach vertex shader from shader program object
    glDetachShader(shaderProgramObject, vertexShaderObject);
    
    // detach fragment shader from shader program object
    glDetachShader(shaderProgramObject, fragmentShaderObject);
    
    // delete vertex shader object
    glDeleteShader(vertexShaderObject);
    vertexShaderObject = 0;
    
    // delete fragment shader object
    glDeleteShader(fragmentShaderObject);
    fragmentShaderObject = 0;
    
    // delete shader program object
    glDeleteProgram(shaderProgramObject);
    shaderProgramObject = 0;
    
    if(depthRenderbuffer)
    {
        glDeleteRenderbuffers(1, &depthRenderbuffer);
        depthRenderbuffer=0;
    }
    
    if(colorRenderbuffer)
    {
        glDeleteRenderbuffers(1, &colorRenderbuffer);
        colorRenderbuffer=0;
    }
    
    if(defaultFramebuffer)
    {
        glDeleteFramebuffers(1, &defaultFramebuffer);
        defaultFramebuffer=0;
    }
    
    if([EAGLContext currentContext]==eaglContext)
    {
        [EAGLContext setCurrentContext:nil];
    }
    [eaglContext release];
    eaglContext=nil;
    
    [super dealloc];
}

@end
