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
    GLuint vao_cube;
    GLuint vbo_pyramid_position;
    GLuint vbo_pyramid_texture;
    GLuint vbo_cube_position;
    GLuint vbo_cube_texture;
    GLuint pyramid_texture;
    GLuint cube_texture;
    GLuint mvpUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
    
    GLuint texture_sampler_uniform;
}

GLfloat anglePyramid=0.0f;
GLfloat angleCube=0.0f;

- (id)initWithFrame:(CGRect)frame
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
        "in vec2 vTexture0_Coord;" \
        "out vec2 out_texture0_coord;" \
        "uniform mat4 u_mvp_matrix;" \
        "void main(void)" \
        "{" \
        "    gl_Position = u_mvp_matrix * vPosition;" \
        "    out_texture0_coord = vTexture0_Coord;" \
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
        "in vec2 out_texture0_coord;" \
        "uniform sampler2D u_texture0_sampler;" \
        "out vec4 FragColor;" \
        "void main(void)" \
        "{" \
        "    FragColor=texture(u_texture0_sampler, out_texture0_coord);" \
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
        glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD0, "vTexture0_Coord");
        
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
        texture_sampler_uniform = glGetUniformLocation(shaderProgramObject,"u_texture0_sampler");
        

        //load textures
        glEnable(GL_TEXTURE_2D);
        pyramid_texture=[self loadTextureFromBMPFile:@"stone" :@"bmp"];
        cube_texture=[self loadTextureFromBMPFile:@"Vijay_Kundali" :@"bmp"];
        
        //vertices, colors, shader attribs, vbo, vao initializations
        const GLfloat pyramidVertices[]=
        {
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
        
        const GLfloat pyramidTexcoords[]=
        {
            // Front face
            0.5f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    
            // Right face
            0.5f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
    
            // Back face
            0.5f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
    
            // Left face
            0.5f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f
        };
        
        GLfloat cubeVertices[]=
        {
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
        
        //If above -1.0f Or +1.0f values make cube much larger than Pyramid,
        //then follow the code in following loop which will convert all 1s And -1s to -0.75 or +0.75
        for(int i = 0; i < 72; i++)
        {
            if(cubeVertices[i] < 0.0f)
                cubeVertices[i] = cubeVertices[i]+0.25f;
            else if(cubeVertices[i] > 0.0f)
                cubeVertices[i] = cubeVertices[i]-0.25f;
            else
                cubeVertices[i] = cubeVertices[i];
        }
        
        const GLfloat cubeTexcoords[]=
        {
            // Top face
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
    
            // Bottom face
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
            1.0f, 0.0f,
    
            // Front face
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
    
            // Back face
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
    
            // right face
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,
            0.0f, 0.0f,
    
            // left face
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };
        
        glGenVertexArrays(1, &vao_pyramid);
        glBindVertexArray(vao_pyramid);
        
        //PYRAMID
        //vbo for position
        glGenBuffers(1, &vbo_pyramid_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_position);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        
        glBindBuffer(GL_ARRAY_BUFFER,0);
        
        //vbo for texture
        glGenBuffers(1, &vbo_pyramid_texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_pyramid_texture);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidTexcoords), pyramidTexcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        glBindVertexArray(0);
        
        //CUBE
        glGenVertexArrays(1, &vao_cube);
        glBindVertexArray(vao_cube);
        
        //vbo for position
        glGenBuffers(1, &vbo_cube_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_position);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        //vbo for texture
        glGenBuffers(1, &vbo_cube_texture);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_texture);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexcoords), cubeTexcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
        
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

-(GLuint)loadTextureFromBMPFile:(NSString *)texFileName :(NSString *)extension
{
    NSString *textureFileNameWithPath=[[NSBundle mainBundle]pathForResource:texFileName ofType:extension];
    UIImage *bmpImage=[[UIImage alloc] initWithContentsOfFile:textureFileNameWithPath];
    if (!bmpImage)
    {
        NSLog(@"can't find %@", textureFileNameWithPath);
        return(0);
    }
    
    CGImageRef cgImage=bmpImage.CGImage;
    int w = (int)CGImageGetWidth(cgImage);
    int h = (int)CGImageGetHeight(cgImage);
    CFDataRef imageData = CGDataProviderCopyData(CGImageGetDataProvider(cgImage));
    void* pixels = (void *)CFDataGetBytePtr(imageData);
    
    GLuint bmpTexture;
    glGenTextures(1, &bmpTexture);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // set 1 rather than default 4, for better performance
    glBindTexture(GL_TEXTURE_2D, bmpTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    // Create mipmaps for this texture for better image quality
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    CFRelease(imageData);
    return(bmpTexture);
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
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    // OpenGL Drawing
    vmath::mat4 modelViewMatrix;
    vmath::mat4 rotationMatrix;
    vmath::mat4 modelViewProjectionMatrix;
    
    //PYRAMID
    //set modelview matrix to identity matrix
    modelViewMatrix=vmath::mat4::identity();
    
    modelViewMatrix=vmath::translate(-1.5f, 0.0f, -6.0f);
    rotationMatrix= vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);
    
    modelViewMatrix= modelViewMatrix * rotationMatrix;
    modelViewProjectionMatrix=perspectiveProjectionMatrix * modelViewMatrix;
    
    glUniformMatrix4fv(mvpUniform,1,GL_FALSE,modelViewProjectionMatrix);
    
    //Work with texture [A][B][U]
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,pyramid_texture);
    glUniform1i(texture_sampler_uniform, 0);
    
    //bind vao
    glBindVertexArray(vao_pyramid);
    
    //draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
    glDrawArrays(GL_TRIANGLES,0,12);
    
    //unbind vao
    glBindVertexArray(0);
    
    //CUBE
    modelViewMatrix=vmath::mat4::identity();
    
    modelViewMatrix=vmath::translate(1.5f, 0.0f, -6.0f);
    rotationMatrix= vmath::rotate(angleCube, angleCube, angleCube);
    
    modelViewMatrix= modelViewMatrix * rotationMatrix;
    modelViewProjectionMatrix=perspectiveProjectionMatrix * modelViewMatrix;
    
    glUniformMatrix4fv(mvpUniform,1,GL_FALSE,modelViewProjectionMatrix);
    
    //Work with texture [A][B][U]
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,cube_texture);
    glUniform1i(texture_sampler_uniform, 0);
    
    //bind vao
    glBindVertexArray(vao_cube);
    
    //draw
    glDrawArrays(GL_TRIANGLE_FAN,0,4);
    glDrawArrays(GL_TRIANGLE_FAN,4,4);
    glDrawArrays(GL_TRIANGLE_FAN,8,4);
    glDrawArrays(GL_TRIANGLE_FAN,12,4);
    glDrawArrays(GL_TRIANGLE_FAN,16,4);
    glDrawArrays(GL_TRIANGLE_FAN,20,4);
    
    //unbind vao
    glBindVertexArray(0);
    
    // stop using OpenGL program object
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    // keep rotating
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
    if(vao_pyramid)
    {
        glDeleteVertexArrays(1, &vao_pyramid);
        vao_pyramid=0;
    }
    
    if(vbo_pyramid_position)
    {
        glDeleteBuffers(1, &vbo_pyramid_position);
        vbo_pyramid_position=0;
    }
    
    if(vbo_pyramid_texture)
    {
        glDeleteBuffers(1, &vbo_pyramid_texture);
        vbo_pyramid_texture=0;
    }
    
    if(pyramid_texture)
    {
        glDeleteTextures(1, &pyramid_texture);
        pyramid_texture=0;
    }
    
    if(vao_cube)
    {
        glDeleteVertexArrays(1, &vao_cube);
        vao_cube=0;
    }
    
    if(vbo_cube_position)
    {
        glDeleteBuffers(1, &vbo_cube_position);
        vbo_cube_position=0;
    }
    
    if(vbo_cube_texture)
    {
        glDeleteBuffers(1, &vbo_cube_texture);
        vbo_cube_texture=0;
    }
    
    if(cube_texture)
    {
        glDeleteTextures(1, &cube_texture);
        cube_texture=0;
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
