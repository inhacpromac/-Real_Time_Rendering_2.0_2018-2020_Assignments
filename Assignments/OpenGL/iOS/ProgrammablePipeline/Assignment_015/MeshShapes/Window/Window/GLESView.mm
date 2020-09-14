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

- (id)initWithFrame:(CGRect)frame;
{
    // code
    giPointElements = 0;
    giLinesElements = 0;
    giSquareElements = 0;
    giDigonalSquareElements = 0;
    giFanElements = 0;
    giColorSquareElements = 0;
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
        "    out_color = vColor;" \
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
    
        /*const GLfloat squareColor[] =
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
        };*/
        
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
        colorSquareIndices[iIndex++] = 17;
        colorSquareIndices[iIndex++] = 16;
        
        colorSquareIndices[iIndex++] = 17;
        colorSquareIndices[iIndex++] = 21;
        colorSquareIndices[iIndex++] = 22;
    
        colorSquareIndices[iIndex++] = 22;
        colorSquareIndices[iIndex++] = 18;
        colorSquareIndices[iIndex++] = 17;
        
        colorSquareIndices[iIndex++] = 18;
        colorSquareIndices[iIndex++] = 22;
        colorSquareIndices[iIndex++] = 23;
    
        colorSquareIndices[iIndex++] = 23;
        colorSquareIndices[iIndex++] = 19;
        colorSquareIndices[iIndex++] = 18;
        
        colorSquareIndices[iIndex++] = 15;
        colorSquareIndices[iIndex++] = 11;
        colorSquareIndices[iIndex++] = 10;
    
        colorSquareIndices[iIndex++] = 10;
        colorSquareIndices[iIndex++] = 14;
        colorSquareIndices[iIndex++] = 15;
        
        colorSquareIndices[iIndex++] = 14;
        colorSquareIndices[iIndex++] = 10;
        colorSquareIndices[iIndex++] = 9;
    
        colorSquareIndices[iIndex++] = 9;
        colorSquareIndices[iIndex++] = 13;
        colorSquareIndices[iIndex++] = 14;
        
        colorSquareIndices[iIndex++] = 13;
        colorSquareIndices[iIndex++] = 9;
        colorSquareIndices[iIndex++] = 8;
    
        colorSquareIndices[iIndex++] = 8;
        colorSquareIndices[iIndex++] = 12;
        colorSquareIndices[iIndex++] = 13;
        
        colorSquareIndices[iIndex++] = 4;
        colorSquareIndices[iIndex++] = 5;
        colorSquareIndices[iIndex++] = 1;
    
        colorSquareIndices[iIndex++] = 1;
        colorSquareIndices[iIndex++] = 0;
        colorSquareIndices[iIndex++] = 4;
        
        colorSquareIndices[iIndex++] = 5;
        colorSquareIndices[iIndex++] = 6;
        colorSquareIndices[iIndex++] = 2;
    
        colorSquareIndices[iIndex++] = 2;
        colorSquareIndices[iIndex++] = 1;
        colorSquareIndices[iIndex++] = 5;
        
        colorSquareIndices[iIndex++] = 6;
        colorSquareIndices[iIndex++] = 7;
        colorSquareIndices[iIndex++] = 3;
    
        colorSquareIndices[iIndex++] = 3;
        colorSquareIndices[iIndex++] = 2;
        colorSquareIndices[iIndex++] = 6;
    
    
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
    //glPointSize(2.5f);
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
