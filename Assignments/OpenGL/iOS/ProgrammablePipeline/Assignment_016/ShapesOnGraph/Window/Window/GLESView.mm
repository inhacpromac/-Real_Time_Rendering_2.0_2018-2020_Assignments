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

const int numberOfCircleLines = 5000;
//const int numberOfCircleVertices = numberOfCircleLines * 2 * 3;
//const int numberOfTriangleVertices = 3 * 3;
//const int numberOfSquareVertices = 4 * 3;
const int numberOfGraphVertices = 1024;
// 21 lines with 2 verticle and horizontal 3 line with 2 middle lines = (21*2*3)+2 = 1024
const float PI = 3.14;
int giNumberOfGraphLines = 0;
int giTotalNumberOfLines = 0;

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
    
    GLuint vao_graph;
    GLuint vbo_graph_position;
    GLuint vbo_graph_color;

    GLuint vao_triangle;
    GLuint vbo_position_triangle;
    GLuint vbo_color_triangle;

    GLuint vao_square;
    GLuint vbo_position_square;
    GLuint vbo_color_square;

    GLuint vao_circle;
    GLuint vbo_circle_position;
    GLuint vbo_circle_color;
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
        "   gl_Position = u_mvp_matrix * vPosition;" \
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
        // Graph
        GLfloat graphVertices[numberOfGraphVertices] = { 0 };
        GLfloat graphColor[numberOfGraphVertices] = { 0 };
        int iArrayIndex = 0;
        float fDifference = 1.0f / 20.0f;
        for (float fIterator = -1.0f; fIterator <= 1.05f; fIterator += fDifference)
        {
            graphColor[iArrayIndex] = 0;
            graphColor[iArrayIndex + 1] = 0;
            graphColor[iArrayIndex + 2] = 1;
    
            graphVertices[iArrayIndex++] = -1.0f;
            graphVertices[iArrayIndex++] = fIterator;
            graphVertices[iArrayIndex++] = 0.0f;
    
            graphColor[iArrayIndex] = 0;
            graphColor[iArrayIndex + 1] = 0;
            graphColor[iArrayIndex + 2] = 1;
    
            graphVertices[iArrayIndex++] = 1.0f;
            graphVertices[iArrayIndex++] = fIterator;
            graphVertices[iArrayIndex++] = 0.0f;
    
            graphColor[iArrayIndex] = 0;
            graphColor[iArrayIndex + 1] = 0;
            graphColor[iArrayIndex + 2] = 1;
    
            graphVertices[iArrayIndex++] = fIterator;
            graphVertices[iArrayIndex++] = -1.0;
            graphVertices[iArrayIndex++] = 0.0f;
    
            graphColor[iArrayIndex] = 0;
            graphColor[iArrayIndex + 1] = 0;
            graphColor[iArrayIndex + 2] = 1;
    
            graphVertices[iArrayIndex++] = fIterator;
            graphVertices[iArrayIndex++] = 1.0;
            graphVertices[iArrayIndex++] = 0.0f;
        }
    
        graphColor[iArrayIndex] = 1;
        graphColor[iArrayIndex + 1] = 0;
        graphColor[iArrayIndex + 2] = 0;
    
        graphVertices[iArrayIndex++] = -1.0f;
        graphVertices[iArrayIndex++] = 0.0f;
        graphVertices[iArrayIndex++] = 0.0f;
    
        graphColor[iArrayIndex] = 1;
        graphColor[iArrayIndex + 1] = 0;
        graphColor[iArrayIndex + 2] = 0;
    
        graphVertices[iArrayIndex++] = 1.0f;
        graphVertices[iArrayIndex++] = 0.0f;
        graphVertices[iArrayIndex++] = 0.0f;
    
        graphColor[iArrayIndex] = 0;
        graphColor[iArrayIndex + 1] = 1;
        graphColor[iArrayIndex + 2] = 0;
    
        graphVertices[iArrayIndex++] = 0.0f;
        graphVertices[iArrayIndex++] = -1.0f;
        graphVertices[iArrayIndex++] = 0.0f;
    
        graphColor[iArrayIndex] = 0;
        graphColor[iArrayIndex + 1] = 1;
        graphColor[iArrayIndex + 2] = 0;
    
        graphVertices[iArrayIndex++] = 0.0f;
        graphVertices[iArrayIndex++] = 1.0f;
        graphVertices[iArrayIndex++] = 0.0f;
    
        giNumberOfGraphLines = iArrayIndex / 2;
    
        // Triangle
        const GLfloat triangleVertices[] =
        {
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };
    
        const GLfloat triangleColor[] =
        {
            1.0, 1.0, 0.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, 0.0
        };
    
        // Square
        const GLfloat squareVertices[] =
        {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };
    
        const GLfloat squareColor[] =
        {
            1.0, 1.0, 0.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, 0.0,
            1.0, 1.0, 0.0
        };
        
        glLineWidth(2.0f);
    
        // Circle
        GLfloat circleVertices[numberOfCircleLines * 2 * 3] = { 0 };
        GLfloat circleColor[numberOfCircleLines * 2 * 3] = { 0 };
    
        iArrayIndex = 0;
        for (int iIterator = 0; iIterator < numberOfCircleLines; iIterator++)
        {
            circleColor[iArrayIndex] = 1.0;
            circleColor[iArrayIndex + 1] = 1.0;
            circleColor[iArrayIndex + 2] = 0.0;
            circleVertices[iArrayIndex++] = (GLfloat)cos((5.0f * PI * iIterator) / numberOfCircleLines);
            circleVertices[iArrayIndex++] = (GLfloat)sin((5.0f * PI * iIterator) / numberOfCircleLines);
            circleVertices[iArrayIndex++] = 0.0f;
    
            iIterator++;
    
            circleColor[iArrayIndex] = 1.0;
            circleColor[iArrayIndex + 1] = 1.0;
            circleColor[iArrayIndex + 2] = 0.0;
            circleVertices[iArrayIndex++] = (GLfloat)cos((5.0f * PI * iIterator) / numberOfCircleLines);
            circleVertices[iArrayIndex++] = (GLfloat)sin((5.0f * PI * iIterator) / numberOfCircleLines);
            circleVertices[iArrayIndex++] = 0.0f;
        }
        
        //Create vao_graph
        glGenVertexArrays(1, &vao_graph);
        glBindVertexArray(vao_graph);
        //Position
        glGenBuffers(1, &vbo_graph_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graph_position);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphVertices), graphVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Color
        glGenBuffers(1, &vbo_graph_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_graph_color);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(graphColor), graphColor, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
    
        //Create vao_triangle
        glGenVertexArrays(1, &vao_triangle);
        glBindVertexArray(vao_triangle);
        //Position
        glGenBuffers(1, &vbo_position_triangle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_triangle);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Color
        glGenBuffers(1, &vbo_color_triangle);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_triangle);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleColor), triangleColor, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
    
        //Create vao_square
        glGenVertexArrays(1, &vao_square);
        glBindVertexArray(vao_square);
        //Position
        glGenBuffers(1, &vbo_position_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_position_square);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Color
        glGenBuffers(1, &vbo_color_square);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_color_square);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(squareColor), squareColor, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    
        glBindVertexArray(0);
    
        //Create vao_circle
        glGenVertexArrays(1, &vao_circle);
        glBindVertexArray(vao_circle);
        //Position
        glGenBuffers(1, &vbo_circle_position);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_position);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), circleVertices, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //Color
        glGenBuffers(1, &vbo_circle_color);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_circle_color);
    
        glBufferData(GL_ARRAY_BUFFER, sizeof(circleColor), circleColor, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_COLOR, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_COLOR);
    
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
    // set modelview & modelviewprojection matrices to identity
    //Declaration of Matrices
    vmath::mat4 modelViewMatrix;
    vmath::mat4 translateMatrix;
    vmath::mat4 scaleMatrix;
    vmath::mat4 modelViewProjectionMatrix;

    //////GRAPH//////

    //Initialize above Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translateMatrix = vmath::translate(0.0f, 0.0f, -3.0f);

    //Do necessary Matrix Multiplication
    modelViewMatrix = modelViewMatrix * translateMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Bind with vao_graph
    glBindVertexArray(vao_graph);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_LINES, 0, giNumberOfGraphLines);

    //Unbind with vao_graph
    glBindVertexArray(0);
    
    //////TRIANGLE//////

    //Initialize above Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translateMatrix = vmath::translate(-0.5f, 0.5f, -3.0f);
    scaleMatrix = vmath::scale(0.5f, 0.5f, 0.0f);

    //Do necessary Matrix Multiplication
    modelViewMatrix = modelViewMatrix * translateMatrix;
    modelViewMatrix = modelViewMatrix * scaleMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Bind with vao_triangle
    glBindVertexArray(vao_triangle);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //Unbind with vao_triangle
    glBindVertexArray(0);

    //////SQUARE//////

    //Initialize above changed Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translateMatrix =  vmath::translate(0.5f, 0.5f, -3.0f);
    scaleMatrix = vmath::scale(0.5f, 0.5f, 0.0f);

    //Do necessary Matrix Multiplication
    modelViewMatrix = modelViewMatrix * translateMatrix;
    modelViewMatrix = modelViewMatrix * scaleMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Bind with vao_square
    glBindVertexArray(vao_square);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    //Unbind with vao_square
    glBindVertexArray(0);

    //////CIRCLE//////

    //Initialize above changed Matrices to identity
    modelViewMatrix = vmath::mat4::identity();
    modelViewProjectionMatrix = vmath::mat4::identity();
    translateMatrix = vmath::mat4::identity();
    scaleMatrix = vmath::mat4::identity();

    //Do necessary Trasformation
    translateMatrix = vmath::translate(0.0f, -0.5f, -3.0f);
    scaleMatrix = vmath::scale(0.5f, 0.5f, 0.5f);

    //Do necessary Matrix Multiplication
    modelViewMatrix = modelViewMatrix * translateMatrix;
    modelViewMatrix = modelViewMatrix * scaleMatrix;
    modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix;

    //Send necessary Matrix to Shader in respective Uniform
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, modelViewProjectionMatrix);

    //Bind with vao_square
    glBindVertexArray(vao_circle);

    //simillar Bind with Texture if any


    //Draw necessary Scence
    glDrawArrays(GL_LINES, 0, numberOfCircleLines);

    //Unbind with vao_square
    glBindVertexArray(0);
    
    // stop using OpenGL program object
    glUseProgram(0);
    
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
    if (vbo_circle_color)
    {
        glDeleteBuffers(1, &vbo_circle_color);
    }
    if (vbo_circle_position)
    {
        glDeleteBuffers(1, &vbo_circle_position);
    }
    if (vao_circle)
    {
        glDeleteBuffers(1, &vao_circle);
    }
    if (vbo_color_square)
    {
        glDeleteBuffers(1, &vbo_color_square);
    }
    if (vbo_position_square)
    {
        glDeleteBuffers(1, &vbo_position_square);
    }
    if (vao_square)
    {
        glDeleteBuffers(1, &vao_square);
    }
    if (vbo_color_triangle)
    {
        glDeleteBuffers(1, &vbo_color_triangle);
    }
    if (vbo_position_triangle)
    {
        glDeleteBuffers(1, &vbo_position_triangle);
    }
    if (vao_triangle)
    {
        glDeleteBuffers(1, &vao_triangle);
    }
    if (vbo_graph_color)
    {
        glDeleteBuffers(1, &vbo_graph_color);
    }
    if (vbo_graph_position)
    {
        glDeleteBuffers(1, &vbo_graph_position);
    }
    if (vao_graph)
    {
        glDeleteBuffers(1, &vao_graph);
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
