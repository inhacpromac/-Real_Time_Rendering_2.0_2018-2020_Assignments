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

#import "sphere.h"

bool isPerVertexLightEnabled = false;
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
        
        //xx//xx//xx//PerVertexInitilization//xx//xx//xx//
        //VERTEX SHADER
        //create shader
        gVertexShaderObjectPerVertex = glCreateShader(GL_VERTEX_SHADER);
        
        //provide source code to shader
        const GLchar *vertexShaderSourceCodePerVertex =
        "#version 300 es" \
        "\n " \
        "precision highp float;" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_m_matrix;" \
        "uniform mat4 u_v_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "uniform mediump int u_lkeypressed;" \
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
                    printf("Vertex Shader PerVertex Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf("Fragment Shader PerVertex Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf("Shader Program PerVertex Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        "#version 300 es" \
        "\n " \
        "precision highp float;" \
        "in vec4 vPosition;" \
        "in vec3 vNormal;" \
        "uniform mat4 u_m_matrix;" \
        "uniform mat4 u_v_matrix;" \
        "uniform mat4 u_p_matrix;" \
        "uniform mediump int u_lkeypressed;" \
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
                    printf("Vertex Shader PerFragment Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        "#version 300 es" \
        "\n" \
        "precision highp float;" \
        "in vec3 tNorm;" \
        "in vec3 light_direction;" \
        "in vec3 viewer_vector;" \
        "uniform mediump int u_lkeypressed;" \
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
                    printf("Fragment Shader PerFragment Compilation Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
                    printf ("Shader Program PerFragment Link Log : %s\n", szInfoLog);
                    free(szInfoLog);
                    [self release];
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
        
        // *** vertices, colors, shader attribs, vbo, vao initializations ***
        makeSphere(1.0, 50, 50);
    
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
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
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
    if(isPerVertexLightEnabled == true)
    {
        isPerVertexLightEnabled = false;
    }
    else if(isPerVertexLightEnabled == false)
    {
        isPerVertexLightEnabled = true;
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
