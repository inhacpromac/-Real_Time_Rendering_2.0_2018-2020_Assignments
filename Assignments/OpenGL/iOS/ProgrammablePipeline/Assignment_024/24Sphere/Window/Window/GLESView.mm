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

bool gbAnimation = false;
bool gbLighting = false;
bool lightXRotate = true;
bool lightYRotate = false;
bool lightZRotate = false;
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
    
    GLuint mUniform;
    GLuint vUniform;
    GLuint pUniform;
    GLuint laUniform;
    GLuint ldUniform;
    GLuint lsUniform;
    GLuint kaUniform;
    GLuint kdUniform;
    GLuint ksUniform;
    GLuint materialShininessUniform;
    GLuint lightPositionUniform;
    GLuint lightingEnabledUniform;
    
    vmath::mat4 perspectiveProjectionMatrix;
}

GLfloat lightAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat lightPosition[] = { 1.0f,1.0f,1.0f,1.0f };

GLfloat angleLight = 0.0f;
GLfloat speed = 0.1f;

const int totalNumberOfRows = 6;
const int totalNumberOfColumns = 4;
GLfloat materialAmbient[totalNumberOfColumns][totalNumberOfRows][4] = {
    {
        {0.0215f, 0.1745f, 0.0215f, 1.0f },
        {0.135f, 0.2225f, 0.1575f, 1.0f },
        {0.05375f, 0.05f, 0.06625f, 1.0f },
        {0.25f, 0.20725f, 0.20725f, 1.0f },
        {0.1745f, 0.01175f, 0.01175f, 1.0f },
        {0.1f, 0.18725f, 0.1745f, 1.0f }
    },
    {
        {0.329412f, 0.223529f, 0.027451f, 1.0f },
        {0.2125f, 0.1275f, 0.054f, 1.0f },
        {0.25f, 0.25f, 0.25f, 1.0f },
        {0.19125f, 0.0735f, 0.0225f, 1.0f },
        {0.24725f, 0.1995f, 0.0745f, 1.0f },
        {0.19225f, 0.19225f, 0.19225f, 1.0f }
    },
    {
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.1f, 0.06f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f },
        {0.0f, 0.0f, 0.0f, 1.0f }
    },
    {
        {0.02f, 0.02f, 0.02f, 1.0f },
        {0.0f, 0.05f, 0.05f, 1.0f },
        {0.0f, 0.05f, 0.0f, 1.0f },
        {0.05f, 0.0f, 0.0f, 1.0f },
        {0.05f, 0.05f, 0.05f, 1.0f },
        {0.05f, 0.05f, 0.0f, 1.0f }
    }
};

GLfloat materialDiffuse[totalNumberOfColumns][totalNumberOfRows][4] = {
    {
        {0.07568f, 0.61424f, 0.07568f, 1.0f},
        {0.54f, 0.89f, 0.63f, 1.0f},
        {0.18275f, 0.17f, 0.22525f, 1.0f},
        {1.0f, 0.829f, 0.829f, 1.0f},
        {0.61424f, 0.04136f, 0.04136f, 1.0f},
        {0.396f, 0.74151f, 0.69102f, 1.0f},
    },
    {
        {0.780392f, 0.568627f, 0.113725f, 1.0f},
        {0.714f, 0.4284f, 0.18144f, 1.0f},
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.7038f, 0.27048f, 0.0828f, 1.0f},
        {0.75164f, 0.60648f, 0.22648f, 1.0f},
        {0.50754f, 0.50754f, 0.50754f, 1.0f},
    },
    {
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.0f, 0.50980392f, 0.50980392f, 1.0f},
        {0.1f, 0.35f, 0.1f, 1.0f},
        {0.5f, 0.0f, 0.0f, 1.0f},
        {0.55f, 0.55f, 0.55f, 1.0f},
        {0.5f, 0.5f, 0.0f, 1.0f},
    },
    {
        {0.01f, 0.01f, 0.01f, 1.0f},
        {0.4f, 0.5f, 0.5f, 1.0f},
        {0.4f, 0.5f, 0.4f, 1.0f},
        {0.5f, 0.4f, 0.4f, 1.0f},
        {0.5f, 0.5f, 0.5f, 1.0f},
        {0.5f, 0.5f, 0.4f, 1.0f},
    },
};

GLfloat materialSpecular[totalNumberOfColumns][totalNumberOfRows][4] = {
    {
        {0.633f, 0.727811f, 0.633f, 1.0f},
        {0.316228f, 0.316228f, 0.316228f, 1.0f},
        {0.332741f, 0.328634f, 0.346435f, 1.0f},
        {0.296648f, 0.296648f, 0.296648f, 1.0f},
        {0.727811f, 0.626959f, 0.626959f, 1.0f},
        {0.297254f, 0.30829f, 0.306678f, 1.0f},
    },
    {
        {0.992157f, 0.941176f, 0.807843f, 1.0f},
        {0.393548f, 0.271906f, 0.166721f, 1.0f},
        {0.774597f, 0.774597f, 0.774597f, 1.0f},
        {0.256777f, 0.137622f, 0.086014f, 1.0f},
        {0.628281f, 0.555802f, 0.366065f, 1.0f},
        {0.508273f, 0.508273f, 0.508273f, 1.0f},
    },
    {
        {0.50f, 0.50f, 0.50f, 1.0f},
        {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
        {0.45f, 0.55f, 0.45f, 1.0f},
        {0.7f, 0.6f, 0.6f, 1.0f},
        {0.70f, 0.70f, 0.70f, 1.0f},
        {0.60f, 0.60f, 0.50f, 1.0f},
    },
    {
        {0.4f, 0.4f, 0.4f, 1.0f},
        {0.04f, 0.7f, 0.7f, 1.0f},
        {0.04f, 0.7f, 0.04f, 1.0f},
        {0.7f, 0.04f, 0.04f, 1.0f},
        {0.7f, 0.7f, 0.7f, 1.0f},
        {0.7f, 0.7f, 0.04f, 1.0f},
    }
};

GLfloat materialShininess[totalNumberOfColumns][totalNumberOfRows] = {
    {
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.3f * 128.0f,
        0.088f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f
    },
    {
        0.21794872f * 128.0f,
        0.2f * 128.0f,
        0.6f * 128.0f,
        0.1f * 128.0f,
        0.4f * 128.0f,
        0.4f * 128.0f
    },
    {
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f,
        0.25f * 128.0f
    },
    {
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f,
        0.078125f * 128.0f
    }
};

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
        "       phong_ads_light = ambient + diffuse + specular;" \
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
        laUniform = glGetUniformLocation(shaderProgramObject, "u_la");
        ldUniform = glGetUniformLocation(shaderProgramObject, "u_ld");
        lsUniform = glGetUniformLocation(shaderProgramObject, "u_ls");
        kaUniform = glGetUniformLocation(shaderProgramObject, "u_ka");
        kdUniform = glGetUniformLocation(shaderProgramObject, "u_kd");
        ksUniform = glGetUniformLocation(shaderProgramObject, "u_ks");
        materialShininessUniform = glGetUniformLocation(shaderProgramObject, "u_material_shininess");
        lightPositionUniform = glGetUniformLocation(shaderProgramObject, "u_light_position");
        
        // *** vertices, colors, shader attribs, vbo, vao initializations ***
        makeSphere(0.5, 50, 50);
    
        // enable depth testing
        glEnable(GL_DEPTH_TEST);
        
        // depth test to do
        glDepthFunc(GL_LEQUAL);
        
        // We will always cull back faces for better performance
        glEnable(GL_CULL_FACE);
        
        //clearcolor
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        
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
//- (void)drawRect:(CGRect)rect
//{
//    // Drawing code
//}

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
    
    //Declaration of Matrices
    vmath::mat4 modelMatrix;
    vmath::mat4 ViewMatrix;
    vmath::mat4 projectionMatrix;
    vmath::mat4 translationMatrix;
    
    for (int column = 0; column < totalNumberOfColumns; ++column)
    {
        for (int row = 0; row < totalNumberOfRows; ++row)
        {
            glViewport(vX + (column * vWidth), vY + (row * vHeight), vWidth, vHeight);

            glUseProgram(shaderProgramObject);

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
                glUniform3fv(laUniform, 1, lightAmbient);
                glUniform3fv(ldUniform, 1, lightDiffuse);
                glUniform3fv(lsUniform, 1, lightSpecular);
                glUniform3fv(kaUniform, 1, materialAmbient[column][row]);
                glUniform3fv(kdUniform, 1, materialDiffuse[column][row]);
                glUniform3fv(ksUniform, 1, materialSpecular[column][row]);
                glUniform1f(materialShininessUniform, materialShininess[column][row]);
                glUniform4fv(lightPositionUniform, 1, lightPosition);
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

        }
    }
    
    glBindRenderbuffer(GL_RENDERBUFFER,colorRenderbuffer);
    [eaglContext presentRenderbuffer:GL_RENDERBUFFER];
    
     if (gbAnimation)
        {
            angleLight += speed;
    
            if (angleLight >= 360.0f)
            {
                angleLight = 0.0f;
            }
            if (lightXRotate)
            {
                lightPosition[0] = 0.0f;
                lightPosition[1] = sinf(angleLight) * 100.0f - 3.0f;
                lightPosition[2] = cosf(angleLight) * 100.0f - 3.0f;
            }
            else if (lightYRotate)
            {
                lightPosition[0] = sinf(angleLight) * 100.0f - 3.0f;
                lightPosition[1] = 0.0f;
                lightPosition[2] = cosf(angleLight) * 100.0f - 3.0f;
            }
            else if (lightZRotate)
            {
                lightPosition[0] = sinf(angleLight) * 100.0f - 3.0f;
                lightPosition[1] = cosf(angleLight) * 100.0f - 3.0f;
                lightPosition[2] = 0.0f;
            }
    
        }
}

int vWidth = 1;
int vHeight = 1;
int vX = 0;
int vY = 0;

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
    
    vWidth = width / totalNumberOfRows;
    vHeight = height / totalNumberOfRows;
    vX = (width - (vWidth * totalNumberOfColumns)) / 2;
    vY = (height - (vHeight * totalNumberOfRows)) / 2;
    
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
    if (lightXRotate == true && lightYRotate == false && lightZRotate == false)
    {
        lightXRotate = false;
        lightYRotate = true;
        lightZRotate = false;
        //break;
    }
    else if (lightXRotate == false && lightYRotate == true && lightZRotate == false)
    {
        lightXRotate = false;
        lightYRotate = false;
        lightZRotate = true;
        //break;
    }
    else if (lightXRotate == false && lightYRotate == false && lightZRotate == true)
    {
        lightXRotate = true;
        lightYRotate = false;
        lightZRotate = false;
        //break;
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
    if (gbAnimation == false)
    {
        gbAnimation = true;
    }
    else if (gbAnimation == true)
    {
        gbAnimation = false;
    }
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
