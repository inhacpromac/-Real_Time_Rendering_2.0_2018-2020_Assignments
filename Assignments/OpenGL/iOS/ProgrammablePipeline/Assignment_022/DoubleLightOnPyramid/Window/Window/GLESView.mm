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

bool gbAnimation = false;
bool gbLighting = false;
float angleCube = 0.0f;

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
    GLuint vbo_normal_pyramid;
    GLuint mUniform;
    GLuint vUniform;
    GLuint pUniform;
    GLuint laZeroUniform;
    GLuint ldZeroUniform;
    GLuint lsZeroUniform;
    GLuint lightZeroPositionUniform;
    GLuint laOneUniform;
    GLuint ldOneUniform;
    GLuint lsOneUniform;
    GLuint lightOnePositionUniform;
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;
    GLuint lightingEnabledUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
}

GLfloat anglePyramid = 0.0f;
GLfloat speed = 1.0f;

GLfloat lightZeroAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroDiffuse[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroSpecular[] = { 1.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightZeroPosition[] = { 2.0f, 1.0f, 1.0f, 0.0f };

GLfloat lightOneAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat lightOneDiffuse[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat lightOneSpecular[] = { 0.0f, 0.0f, 1.0f, 0.0f };
GLfloat lightOnePosition[] = { -2.0f, 1.0f, 1.0f, 0.0f };

GLfloat materialAmbient[] = { 0.0f, 0.0f, 0.0f, 0.0f };
GLfloat materialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 0.0f };
GLfloat materialShininess = 128.0f;    //50.0f

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
        "in vec3 vNormal;" \
        "uniform mat4 u_m_matrix;" \
        "uniform mat4 u_v_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "uniform mediump int u_lkeypressed;" \
        "uniform vec3 u_la_zero;" \
        "uniform vec3 u_ld_zero;" \
        "uniform vec3 u_ls_zero;" \
        "uniform vec3 u_la_one;" \
        "uniform vec3 u_ld_one;" \
        "uniform vec3 u_ls_one;" \
        "uniform vec3 u_ka;" \
        "uniform vec3 u_kd;" \
        "uniform vec3 u_ks;" \
        "uniform vec4 u_light_position_zero;" \
        "uniform vec4 u_light_position_one;" \
        "uniform float u_material_shininess;" \
        "out vec3 phong_ads_light;" \
        "void main(void)" \
        "{" \
        "   if(u_lkeypressed == 1)" \
        "   {" \
        "       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" \
        "       vec3 tNorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" \
        "       vec3 light_direction_zero = normalize(vec3(u_light_position_zero - eye_coordinates));" \
        "       vec3 light_direction_one = normalize(vec3(u_light_position_one - eye_coordinates));" \
        "       float tn_dot_ld_zero = max(dot(light_direction_zero, tNorm), 0.0);" \
        "       float tn_dot_ld_one = max(dot(light_direction_one, tNorm), 0.0);" \
        "       vec3 reflection_vector_zero = reflect(-light_direction_zero, tNorm);" \
        "       vec3 reflection_vector_one = reflect(-light_direction_one, tNorm);" \
        "       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" \
        "       vec3 ambient_zero = u_la_zero * u_ka;" \
        "       vec3 ambient_one = u_la_one * u_ka;" \
        "       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" \
        "       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" \
        "       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, viewer_vector), 0.0), u_material_shininess);" \
        "       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, viewer_vector), 0.0), u_material_shininess);" \
        "       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one;"
        "   }" \
        "   else" \
        "   {" \
        "       phong_ads_light = vec3(1.0, 1.0, 1.0);" \
        "   }" \
        "   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" \
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
        "in vec3 phong_ads_light;" \
        "out vec4 FragColor;" \
        "uniform mediump int u_lkeypressed;" \
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
        glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "vNormal");
        
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
        kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
        kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
        ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
        materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        lightZeroPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position_zero");
        lightOnePositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position_one");
        
        //vertices, colors, shader attribs, vbo, vao initializations
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
    
        const GLfloat pyramidNormals[] = {
            // Front face
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,
            0.0f, 0.447214f, 0.894427f,
    
            // Right face
            0.894427f, 0.447214f, 0.0f,
            0.894427f, 0.447214f, 0.0f,
            0.894427f, 0.447214f, 0.0f,
    
            // Back face
            0.0f, 0.447214f, -0.894427f,
            0.0f, 0.447214f, -0.894427f,
            0.0f, 0.447214f, -0.894427f,
    
            // Left face
            -0.894427f, 0.447214f, 0.0f,
            -0.894427f, 0.447214f, 0.0f,
            -0.894427f, 0.447214f, 0.0f
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
        //Normal
        glGenBuffers(1, &vbo_normal_pyramid);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normal_pyramid);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidNormals), pyramidNormals, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
    
        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        
        // depth test to do
        glDepthFunc(GL_LEQUAL);
        
        // We will always cull back faces for better performance
        glEnable(GL_CULL_FACE);
        
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
    [EAGLContext setCurrentContext:eaglContext];
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    // start using OpenGL program object
    glUseProgram(shaderProgramObject);
    
    // OpenGL Drawing
    //Declaration of Matrices
    vmath::mat4 modelMatrix;
    vmath::mat4 ViewMatrix;
    vmath::mat4 projectionMatrix;
    vmath::mat4 translationMatrix;
    vmath::mat4 rotationMatrix;

    //Initialize above changed Matrices to identity
    modelMatrix = vmath::mat4::identity();
    ViewMatrix = vmath::mat4::identity();
    projectionMatrix = vmath::mat4::identity();
    translationMatrix = vmath::mat4::identity();
    rotationMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
    rotationMatrix = vmath::rotate(anglePyramid, 0.0f, 1.0f, 0.0f);

    //Do necessary Matrix Multiplication
    modelMatrix = modelMatrix * translationMatrix;
    modelMatrix = modelMatrix * rotationMatrix;
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
        glUniform3fv(kaUniform, 1, materialAmbient);
        glUniform3fv(kdUniform, 1, materialDiffuse);
        glUniform3fv(ksUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
        glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition);
        glUniform4fv(lightOnePositionUniform, 1, lightOnePosition);
    }
    else
    {
        glUniform1i(lightingEnabledUniform, 0);
    }
    
    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mUniform, 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(vUniform, 1, GL_FALSE, ViewMatrix);
    glUniformMatrix4fv(pUniform, 1, GL_FALSE, projectionMatrix);

    //Bind with vao
    glBindVertexArray(vao_pyramid);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_TRIANGLES, 0, 12);

    //Unbind with vao
    glBindVertexArray(0);
    
    // stop using OpenGL program object
    glUseProgram(0);
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
    if(gbAnimation == true)
    {
        anglePyramid += speed;
        
        if (anglePyramid >= 360.0f)
        {
            anglePyramid = 0.0f;
        }
    }
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
    if (gbAnimation == false)
    {
        gbAnimation = true;
    }
    else if (gbAnimation == true)
    {
        gbAnimation = false;
    }
}

-(void)onDoubleTap:(UITapGestureRecognizer *)gr
{
    if (gbLighting == false)
    {
        gbLighting = true;
    }
    else if (gbLighting == true)
    {
        gbLighting = false;
    }
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
    if (vbo_normal_pyramid)
    {
        glDeleteBuffers(1, &vbo_normal_pyramid);
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
