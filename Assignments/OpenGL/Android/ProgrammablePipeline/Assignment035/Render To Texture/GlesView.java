package com.inhacpromac.rendertotexture;

//added by inhacpromac
//for context
import android.content.Context;


//for such a view which will give OpenGL supported surface
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;


//for touchevent
import android.view.MotionEvent;
//for gesture event
import android.view.GestureDetector;
//for gesture event's OnGestureListener
import android.view.GestureDetector.OnGestureListener;
//for gesture event's OnDoubleTapListener
import android.view.GestureDetector.OnDoubleTapListener;


//for basic feature of OpenGLES
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;


//for OpenGL Buffers
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;


//for Matrix math
import android.opengl.Matrix;

//for Sphere Geometry
import java.nio.ShortBuffer;

//for Maths
import java.lang.Math;

//implements OnGestureListener, OnDoubleTapListener :means
//all events in this class are manditory to use even though it is empty
//implementing inner class of GLSurfaceView, Renderer
public class GlesView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
    private GestureDetector gestureDetector;
	
	private boolean gbAnimation = false;
	private boolean gbLighting = false;
	private boolean lightXRotate = true;
	private boolean lightYRotate = false;
	private boolean lightZRotate = false;
	
	//java does not have Uint nor GLuint
	private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    private int shaderProgramObjectCube;
	
	//Cube
    private int[] vao_cube = new int[1];
	private int[] vbo_position_cube= new int[1];
	private int[] vbo_color_cube= new int[1];
	private int[] vbo_texture_cube= new int[1];
	private int[] texture_kundali = new int[1];
	private int samplerUniform;
	
	int glWidth;
	int glHeight;
	
	private int mvpUniform;
	
	private float angleCube = 0.0f;
	private float speedCube = 1.0f;

	//for framebuffer
	private int[] fbo = new int[1];
	private int[] color_texture = new int[1];
	private int[] depth_texture = new int[1];
	
	
	private int incdecClearColor = 0;
	private float redClearColor = 1.0f;
	private float greenClearColor = 1.0f;
	private float blueClearColor = 1.0f;
	
	
	//java does not have address oprator hence we give array of 1 and pass it's name as address
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	private int mUniform;
	private int vUniform;
	private int pUniform;
	private int laUniform = 0;
    private int ldUniform = 0;
    private int lsUniform = 0;
    private int kaUniform = 0;
    private int kdUniform = 0;
    private int ksUniform = 0;
    private int materialShininessUniform = 0;
    private int lightPositionUniform = 0;
    private int lightingEnabledUniform = 0;
	private int numVertices;
	private int numElements;
	private int totalNumberOfRows = 6;
	private int totalNumberOfColumns = 4;
	
	private float angleLight = 0.0f;
    private float speed = 0.1f;
	
	private float[] lightAmbient = { 0.0f,0.0f,0.0f,1.0f };
    private float[] lightDiffuse = { 1.0f,1.0f,1.0f,1.0f };
    private float[] lightSpecular = { 1.0f,1.0f,1.0f,1.0f };
    private float[] lightPosition = { 100.0f,100.0f,100.0f,1.0f };

    private float materialAmbient[][][] = {
        {   // Column 1
            {0.0215f, 0.1745f, 0.0215f, 1.0f },
            {0.135f, 0.2225f, 0.1575f, 1.0f },
            {0.05375f, 0.05f, 0.06625f, 1.0f },
            {0.25f, 0.20725f, 0.20725f, 1.0f },
            {0.1745f, 0.01175f, 0.01175f, 1.0f },
            {0.1f, 0.18725f, 0.1745f, 1.0f }
        },
        {   // Column 2
            {0.329412f, 0.223529f, 0.027451f, 1.0f },
            {0.2125f, 0.1275f, 0.054f, 1.0f },
            {0.25f, 0.25f, 0.25f, 1.0f },
            {0.19125f, 0.0735f, 0.0225f, 1.0f },
            {0.24725f, 0.1995f, 0.0745f, 1.0f },
            {0.19225f, 0.19225f, 0.19225f, 1.0f }
        },
        {   // Column 3
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.1f, 0.06f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f },
            {0.0f, 0.0f, 0.0f, 1.0f }
        },
        {   // Column 4
            {0.02f, 0.02f, 0.02f, 1.0f },
            {0.0f, 0.05f, 0.05f, 1.0f },
            {0.0f, 0.05f, 0.0f, 1.0f },
            {0.05f, 0.0f, 0.0f, 1.0f },
            {0.05f, 0.05f, 0.05f, 1.0f },
            {0.05f, 0.05f, 0.0f, 1.0f }
        }
    };

    private float materialDiffuse[][][] = {
        {   // Column 1
            {0.07568f, 0.61424f, 0.07568f, 1.0f},
            {0.54f, 0.89f, 0.63f, 1.0f},
            {0.18275f, 0.17f, 0.22525f, 1.0f},
            {1.0f, 0.829f, 0.829f, 1.0f},
            {0.61424f, 0.04136f, 0.04136f, 1.0f},
            {0.396f, 0.74151f, 0.69102f, 1.0f},
        },
        {   // Column 2
            {0.780392f, 0.568627f, 0.113725f, 1.0f},
            {0.714f, 0.4284f, 0.18144f, 1.0f},
            {0.4f, 0.4f, 0.4f, 1.0f},
            {0.7038f, 0.27048f, 0.0828f, 1.0f},
            {0.75164f, 0.60648f, 0.22648f, 1.0f},
            {0.50754f, 0.50754f, 0.50754f, 1.0f},
        },
        {   // Column 3
            {0.01f, 0.01f, 0.01f, 1.0f},
            {0.0f, 0.50980392f, 0.50980392f, 1.0f},
            {0.1f, 0.35f, 0.1f, 1.0f},
            {0.5f, 0.0f, 0.0f, 1.0f},
            {0.55f, 0.55f, 0.55f, 1.0f},
            {0.5f, 0.5f, 0.0f, 1.0f},
        },
        {   // Column 4
            {0.01f, 0.01f, 0.01f, 1.0f},
            {0.4f, 0.5f, 0.5f, 1.0f},
            {0.4f, 0.5f, 0.4f, 1.0f},
            {0.5f, 0.4f, 0.4f, 1.0f},
            {0.5f, 0.5f, 0.5f, 1.0f},
            {0.5f, 0.5f, 0.4f, 1.0f},
        },
    };

    private float materialSpecular[][][] = {
        {   // Column 1
            {0.633f, 0.727811f, 0.633f, 1.0f},
            {0.316228f, 0.316228f, 0.316228f, 1.0f},
            {0.332741f, 0.328634f, 0.346435f, 1.0f},
            {0.296648f, 0.296648f, 0.296648f, 1.0f},
            {0.727811f, 0.626959f, 0.626959f, 1.0f},
            {0.297254f, 0.30829f, 0.306678f, 1.0f},
        },
        {   // Column 2
            {0.992157f, 0.941176f, 0.807843f, 1.0f},
            {0.393548f, 0.271906f, 0.166721f, 1.0f},
            {0.774597f, 0.774597f, 0.774597f, 1.0f},
            {0.256777f, 0.137622f, 0.086014f, 1.0f},
            {0.628281f, 0.555802f, 0.366065f, 1.0f},
            {0.508273f, 0.508273f, 0.508273f, 1.0f},
        },
        {   // Column 3
            {0.50f, 0.50f, 0.50f, 1.0f},
            {0.50196078f, 0.50196078f, 0.50196078f, 1.0f},
            {0.45f, 0.55f, 0.45f, 1.0f},
            {0.7f, 0.6f, 0.6f, 1.0f},
            {0.70f, 0.70f, 0.70f, 1.0f},
            {0.60f, 0.60f, 0.50f, 1.0f},
        },
        {   // Column 4
            {0.4f, 0.4f, 0.4f, 1.0f},
            {0.04f, 0.7f, 0.7f, 1.0f},
            {0.04f, 0.7f, 0.04f, 1.0f},
            {0.7f, 0.04f, 0.04f, 1.0f},
            {0.7f, 0.7f, 0.7f, 1.0f},
            {0.7f, 0.7f, 0.04f, 1.0f},
        }
    };

    private float materialShininess[][] = {
        {   // Column 1
            0.6f * 128.0f,
            0.1f * 128.0f,
            0.3f * 128.0f,
            0.088f * 128.0f,
            0.6f * 128.0f,
            0.1f * 128.0f
        },
        {   // Column 2
            0.21794872f * 128.0f,
            0.2f * 128.0f,
            0.6f * 128.0f,
            0.1f * 128.0f,
            0.4f * 128.0f,
            0.4f * 128.0f
        },
        {   // Column 3
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f,
            0.25f * 128.0f
        },
        {   // Column 4
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f,
            0.078125f * 128.0f
        }
    };

    private float[] perspectiveProjectionMatrix = new float[16];//4X4 matrix

    public GlesView(Context drawingContext) {

        super(drawingContext);
		
		context = drawingContext;
		
		setEGLContextClientVersion(3);
        setRenderer(this);
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        gestureDetector = new GestureDetector(drawingContext, this, null, false);
        gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {

        //this is most important line but not in use under openGL programming.
        //this line is for keyboard gesture.
        int eventAction = event.getAction();

        if (!gestureDetector.onTouchEvent(event)) {
            super.onTouchEvent(event);
        }

        return true;
    }

    //OnDoubleTapListener
    @Override
    public boolean onDoubleTap(MotionEvent event) {
		if(lightXRotate == true && lightYRotate == false && lightZRotate == false)
		{
			lightXRotate = false;
			lightYRotate = true;	//Yrotation
			lightZRotate = false;
			//break;
		}
		else if(lightXRotate == false && lightYRotate == true && lightZRotate == false)
		{
			lightXRotate = false;
			lightYRotate = false;
			lightZRotate = true;	//Zrotation
			//break;
		}
		else if(lightXRotate == false && lightYRotate == false && lightZRotate == true)
		{
			lightXRotate = true;	//Xrotation
			lightYRotate = false;
			lightZRotate = false;
			//break;
		}
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
		if(gbAnimation == false)
		{
			gbAnimation = true;
			gbLighting = true;
			//redClearColor = 0.0f;
			//greenClearColor = 0.0f;
			//blueClearColor = 0.0f;
		}
		else
		{
			gbAnimation = false;
			//gbLighting = false;
		}
        return true;
    }

    //OnGestureListener
    @Override
    public boolean onDown(MotionEvent event) {
        return true;
    }

    @Override
    public boolean onFling(MotionEvent event1, MotionEvent event2, float velocityX, float velocityY) {
        return true;
    }

    @Override
    public void onLongPress(MotionEvent event) {
		// if(gbLighting == false)
		// {
			// gbLighting = true;
		// }
		// else
		// {
			// gbLighting = false;
		// }
    }

    @Override
    public boolean onScroll(MotionEvent event1, MotionEvent event2, float distanceX, float distanceY) {
		uninitialize();
        System.exit(0);
        return true;
    }

    @Override
    public void onShowPress(MotionEvent event) {

    }

    @Override
    public boolean onSingleTapUp(MotionEvent event) {
        return true;
    }
	
	//Implement GLSurfaceView.Renderer methods
	@Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String version = gl.glGetString(GL10.GL_VERSION);
		System.out.println("INHACPROMAC_OpenGL_version: " + version);
		
		String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("INHACPROMAC_GLSL_version: " + glslVersion);
		
		String vendor = gl.glGetString(GL10.GL_VENDOR);
		System.out.println("INHACPROMAC_OpenGL_Vendor: " + vendor);
		
		String renderer = gl.glGetString(GL10.GL_RENDERER);
		System.out.println("INHACPROMAC_OpenGL_Renderer: " + renderer);

        initialize();
    }
	//ususally don't use OpenGL fuctions is resemble by GL10 unused..
    @Override
    public void onSurfaceChanged(GL10 unused, int width, int height) {
		glWidth = width;
		glHeight = height;
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        display();
    }
	
	private void initialize_cube() {
		///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
		//Define Vertex Shader Object
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		//Write Vertex Shader Object
		final String vertexShaderSourceCode = String.format
		(	
			"#version 320 es" +
			"\n" +
			"in vec4 vPosition;" +
			"/*in vec4 vColor;*/" +
			"in vec2 vTexCoord;" +
			"uniform mat4 u_mvp_matrix;" +
			"out vec4 out_color;" +
			"out vec2 out_texcoord;" +
			"void main(void)" +
			"{" +
			"   gl_Position = u_mvp_matrix * vPosition;" +
			"	/*out_color = vColor;*/" +
			"	out_texcoord = vTexCoord;" +
			"}"
		);
		//Specifying Above code to the VertexShaderObject
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		//Compile the Vertex Shader
		GLES32.glCompileShader(vertexShaderObject);
		//Error Checking for Vertex Shader Compiling
		int[]  iShaderCompileStatus = new int[1];
		int[]  iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("INHACPROMAC | Vertex Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
		//Define Fragment Shader Object
		fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		//Write Fragment Shader Object
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es" +
			"\n" +
			"precision highp float;" +
			"/*in vec4 out_color;*/" +
			"in vec2 out_texcoord;" +
			"uniform sampler2D u_sampler;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"	FragColor = /*out_color */ texture(u_sampler, out_texcoord);" +
			"}"
		);
		//Specifying Above code to the FragmentShaderObject
		GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
		//Compile the fragment Shader
		GLES32.glCompileShader(fragmentShaderObject);
		//Error Checking for Fragment Shader Compiling
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("INHACPROMAC | Fragment Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
		//Create Shader Program Object
		shaderProgramObjectCube = GLES32.glCreateProgram();
		//Attach Vertex Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObjectCube, vertexShaderObject);
		//Attach Fragment Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObjectCube, fragmentShaderObject);
		//preLinking Binding to Vertex Attributes
		GLES32.glBindAttribLocation(shaderProgramObjectCube, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectCube, GLESMacros.AMC_ATTRIBUTE_COLOR, "vColor");
		GLES32.glBindAttribLocation(shaderProgramObjectCube, GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
		//Link The Shader Program
		GLES32.glLinkProgram(shaderProgramObjectCube);
		//Error Checking for Shader Program Linking
		int[] iProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetProgramiv(shaderProgramObjectCube, GLES32.GL_LINK_STATUS, iProgramLinkStatus, 0);
		if (iProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectCube, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				GLES32.glGetProgramInfoLog(shaderProgramObjectCube);
				System.out.println("INHACPROMAC | Shader Program Link Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		//postLinking retriving uniform location
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObjectCube, "u_mvp_matrix");
		samplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_sampler");
		///////////////////////////////////////////////////////////////////////////////////////////////

		final float[] cubeVertices = new float[]
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
		
		// final float[] cubeColor = new float[]
		// {
			// // Top face
			// 1.0f, 0.0f, 0.0f,
			// 1.0f, 0.0f, 0.0f,
			// 1.0f, 0.0f, 0.0f,
			// 1.0f, 0.0f, 0.0f,

			// // Bottom face
			// 0.0f, 1.0f, 0.0f,
			// 0.0f, 1.0f, 0.0f,
			// 0.0f, 1.0f, 0.0f,
			// 0.0f, 1.0f, 0.0f,

			// // Front face
			// 0.0f, 0.0f, 1.0f,
			// 0.0f, 0.0f, 1.0f,
			// 0.0f, 0.0f, 1.0f,
			// 0.0f, 0.0f, 1.0f,

			// // Back face
			// 0.0f, 1.0f, 1.0f,
			// 0.0f, 1.0f, 1.0f,
			// 0.0f, 1.0f, 1.0f,
			// 0.0f, 1.0f, 1.0f,

			// // right face
			// 1.0f, 0.0f, 1.0f,
			// 1.0f, 0.0f, 1.0f,
			// 1.0f, 0.0f, 1.0f,
			// 1.0f, 0.0f, 1.0f,

			// // left face
			// 1.0f, 1.0f, 0.0f,
			// 1.0f, 1.0f, 0.0f,
			// 1.0f, 1.0f, 0.0f,
			// 1.0f, 1.0f, 0.0f
		// };
		
		final float[] cubeTexCoord = new float[]
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
		
		//Create vao_cube
		GLES32.glGenVertexArrays(1, vao_cube, 0);
		GLES32.glBindVertexArray(vao_cube[0]);
		//Position
		GLES32.glGenBuffers(1, vbo_position_cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_cube[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cubeVertices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(cubeVertices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Color
		// GLES32.glGenBuffers(1, vbo_color_cube, 0);
		// GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_cube[0]);
		
		// //Convert the array into compatible buffer such the we can pass it through to BufferData
		
		// //1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		// byteBuffer = ByteBuffer.allocateDirect(cubeColor.length * 4);//(ArrayLength*DatatypeSize)
		// //2: Arrange the byte order of buffer in native byte order.
        // byteBuffer.order(ByteOrder.nativeOrder());
		// //3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        // positionBuffer = byteBuffer.asFloatBuffer();
		// //4: Now, put your array into "cooked buffer".
        // positionBuffer.put(cubeColor);
		// //5: Set the array at "0TH" position of the buffer.
        // positionBuffer.position(0);

		// GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		// GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		// GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);

		// GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//texture
		GLES32.glGenBuffers(1, vbo_texture_cube, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture_cube[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(cubeTexCoord.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        positionBuffer = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(cubeTexCoord);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);
		
		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cubeTexCoord.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
	}
	
	private void initialize() {
		initialize_cube();
		
		///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
		//Define Vertex Shader Object
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		//Write Vertex Shader Object
		final String vertexShaderSourceCode = String.format
		(	
			"#version 320 es" +
			"\n" +
			"in vec4 vPosition;" +
			"in vec3 vNormal;" +
			"uniform mat4 u_m_matrix;" +
			"uniform mat4 u_v_matrix;" +
			"uniform mat4 u_p_matrix;" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec4 u_light_position;" +
			"out vec3 tNorm;" +
			"out vec3 light_direction;" +
			"out vec3 viewer_vector;" +
			"void main(void)" +
			"{" +
			"   if(u_lkeypressed == 1)" +
			"   {" +
			"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
			"       tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" +
			"       light_direction = vec3(u_light_position - eye_coordinates);" +
			"       viewer_vector = -eye_coordinates.xyz;" +
			"   }" +
			"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" +
			"}"
		);
		//Specifying Above code to the VertexShaderObject
		GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
		//Compile the Vertex Shader
		GLES32.glCompileShader(vertexShaderObject);
		//Error Checking for Vertex Shader Compiling
		int[]  iShaderCompileStatus = new int[1];
		int[]  iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
				System.out.println("INHACPROMAC | Vertex Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
		//Define Fragment Shader Object
		fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		//Write Fragment Shader Object
		final String fragmentShaderSourceCode = String.format
		(
			"#version 320 es" +
			"\n" +
			"precision highp float;" +
			"in vec3 tNorm;" +
			"in vec3 light_direction;" +
			"in vec3 viewer_vector;" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec3 u_la;" +
			"uniform vec3 u_ld;" +
			"uniform vec3 u_ls;" +
			"uniform vec3 u_ka;" +
			"uniform vec3 u_kd;" +
			"uniform vec3 u_ks;" +
			"uniform float u_material_shininess;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"   vec3 phong_ads_light;" +
			"   if(u_lkeypressed == 1)" +
			"   {" +
			"       vec3 normalized_tNorm = normalize(tNorm);" +
			"       vec3 normalized_light_direction = normalize(light_direction);" +
			"       vec3 normalized_viewer_vector = normalize(viewer_vector);" +
			"       float tn_dot_ld = max(dot(normalized_light_direction, normalized_tNorm), 0.0);" +
			"       vec3 reflection_vector = reflect(-normalized_light_direction, normalized_tNorm);" +
			"       vec3 ambient = u_la * u_ka;" +
			"       vec3 diffuse = u_ld * u_kd * tn_dot_ld;" +
			"       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, normalized_viewer_vector), 0.0), u_material_shininess);" +
			"       phong_ads_light = ambient + diffuse + specular;" +
			"   }" +
			"   else" +
			"   {" +
			"       phong_ads_light = vec3(1.0, 1.0, 1.0);" +
			"   }" +
			"	FragColor = vec4(phong_ads_light, 1.0);" +
			"}"
		);
		//Specifying Above code to the FragmentShaderObject
		GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
		//Compile the fragment Shader
		GLES32.glCompileShader(fragmentShaderObject);
		//Error Checking for Fragment Shader Compiling
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
				System.out.println("INHACPROMAC | Fragment Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
		//Create Shader Program Object
		shaderProgramObject = GLES32.glCreateProgram();
		//Attach Vertex Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
		//Attach Fragment Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		//preLinking Binding to Vertex Attributes
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
		//Link The Shader Program
		GLES32.glLinkProgram(shaderProgramObject);
		//Error Checking for Shader Program Linking
		int[] iProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, iProgramLinkStatus, 0);
		if (iProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("INHACPROMAC | Shader Program Link Log: " + szInfoLog);
				uninitialize();
				System.exit(1);
			}
		}
		//postLinking retriving uniform location
		mUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_m_matrix");
		vUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_v_matrix");
		pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_p_matrix");
		laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la");
		ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld");
		lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls");
		kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");
		materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");
		lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position");
		lightingEnabledUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_lkeypressed");
		///////////////////////////////////////////////////////////////////////////////////////////////
		Sphere sphere=new Sphere();
        float sphere_vertices[]=new float[1146];
        float sphere_normals[]=new float[1146];
        float sphere_textures[]=new float[764];
        short sphere_elements[]=new short[2280];
        sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
        numVertices = sphere.getNumberOfSphereVertices();
        numElements = sphere.getNumberOfSphereElements();

        // vao
        GLES32.glGenVertexArrays(1,vao_sphere,0);
        GLES32.glBindVertexArray(vao_sphere[0]);
        
        // position vbo
        GLES32.glGenBuffers(1,vbo_sphere_position,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
        
        ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_vertices);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_vertices.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // normal vbo
        GLES32.glGenBuffers(1,vbo_sphere_normal,0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
        byteBuffer.order(ByteOrder.nativeOrder());
        verticesBuffer=byteBuffer.asFloatBuffer();
        verticesBuffer.put(sphere_normals);
        verticesBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                            sphere_normals.length * 4,
                            verticesBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL,
                                     3,
                                     GLES32.GL_FLOAT,
                                     false,0,0);
        
        GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);
        
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
        
        // element vbo
        GLES32.glGenBuffers(1, vbo_sphere_element, 0);
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
        
        byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
        byteBuffer.order(ByteOrder.nativeOrder());
        ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
        elementsBuffer.put(sphere_elements);
        elementsBuffer.position(0);
        
        GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                            sphere_elements.length * 2,
                            elementsBuffer,
                            GLES32.GL_STATIC_DRAW);
        
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

        GLES32.glBindVertexArray(0);
		
		
		
		
		//FrameBufferObjects
		GLES32.glGenFramebuffers(1, fbo, 0);
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);
		
		GLES32.glGenTextures(1, color_texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, color_texture[0]);
		GLES32.glTexStorage2D(GLES32.GL_TEXTURE_2D, 1, GLES32.GL_RGBA8, 512, 512);// only mem 
		
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);
		GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
		
		GLES32.glGenTextures(1, depth_texture, 0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, depth_texture[0]);
		GLES32.glTexStorage2D(GLES32.GL_TEXTURE_2D, 1, GLES32.GL_DEPTH_COMPONENT32F, 512, 512);
		
		GLES32.glFramebufferTexture(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, color_texture[0], 0);
		GLES32.glFramebufferTexture(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, depth_texture[0], 0);

		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }
	
	private int vWidth = 1;
	private int vHeight = 1;
	private int vX = 0;
	private int vY = 0;
	
	private void resize(int width, int height) {
		if (height == 0)
		{
			height = 1;
		}
		vWidth = width / totalNumberOfColumns;
		vHeight = height / totalNumberOfRows;
		vX = (width - (vWidth * totalNumberOfColumns)) / 2;
		vY = (height - (vHeight * totalNumberOfRows)) / 2;
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
    }

    private void display() {
		
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);
		GLES32.glViewport(0, 0, 512, 512);
		
		//GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		GLES32.glClearColor(redClearColor, greenClearColor, blueClearColor, 1.0f);
		
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		
		GLES32.glUseProgram(shaderProgramObject);
		
		vWidth = 512 / totalNumberOfColumns;
		vHeight = 512 / totalNumberOfRows;
		vX = (512 - (vWidth * totalNumberOfColumns)) / 2;
		vY = (512 - (vHeight * totalNumberOfRows)) / 2;
		
		for (int column = 0; column < totalNumberOfColumns; ++column)
		{
			for (int row = 0; row < totalNumberOfRows; ++row)
			{
				GLES32.glViewport(vX + (column * vWidth), vY + (row * vHeight), vWidth, vHeight);
				

				//Declaration of Matrices
				float[] modelMatrix = new float[16];
				float[] viewMatrix = new float[16];
				float[] projectionMatrix = new float[16];
				float[] translationMatrix = new float[16];

				//Initialize above Matrices to identity
				Matrix.setIdentityM(modelMatrix, 0);
				Matrix.setIdentityM(viewMatrix, 0);
				Matrix.setIdentityM(projectionMatrix, 0);
				Matrix.setIdentityM(translationMatrix, 0);
				
				//Do necessary Trasformation
				Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);

				//Do necessary Matrix Multiplication
				Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
				Matrix.multiplyMM(projectionMatrix, 0, perspectiveProjectionMatrix, 0, projectionMatrix, 0);
				
				if(gbLighting)
				{
					GLES32.glUniform1i(lightingEnabledUniform, 1);
					GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
					GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
					GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
					if (lightXRotate)
					{
						lightPosition[0] = 0.0f;
						lightPosition[1] = (float)Math.sin(angleLight) * 100.0f - 3.0f;
						lightPosition[2] = (float)Math.cos(angleLight) * 100.0f - 3.0f;
					}
					else if (lightYRotate)
					{
						lightPosition[0] = (float)Math.sin(angleLight) * 100.0f - 3.0f;
						lightPosition[1] = 0.0f;
						lightPosition[2] = (float)Math.cos(angleLight) * 100.0f - 3.0f;
					}
					else if (lightZRotate)
					{
						lightPosition[0] = (float)Math.sin(angleLight) * 100.0f - 3.0f;
						lightPosition[1] = (float)Math.cos(angleLight) * 100.0f - 3.0f;
						lightPosition[2] = 0.0f;
					}
					GLES32.glUniform3fv(kaUniform, 1, materialAmbient[column][row], 0);
                    GLES32.glUniform3fv(kdUniform, 1, materialDiffuse[column][row], 0);
                    GLES32.glUniform3fv(ksUniform, 1, materialSpecular[column][row], 0);
					GLES32.glUniform1f(materialShininessUniform, materialShininess[column][row]);
					GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);
				}
				else
				{
					GLES32.glUniform1i(lightingEnabledUniform, 0);
				}

				GLES32.glUniformMatrix4fv(mUniform, 1, false, modelMatrix, 0);
				GLES32.glUniformMatrix4fv(vUniform, 1, false, viewMatrix, 0);
				GLES32.glUniformMatrix4fv(pUniform, 1, false, projectionMatrix, 0);

				//Bind with vao
				GLES32.glBindVertexArray(vao_sphere[0]);
				
				//Draw necessary Scence
				// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
				GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
				GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
				
				//simillar Bind with Texture if any


				//Unbind with vao
				GLES32.glBindVertexArray(0);
			}
		}
		
		GLES32.glUseProgram(0);
		
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);
		
		GLES32.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
		GLES32.glViewport(0, 0, glWidth, glHeight);
		
		GLES32.glUseProgram(shaderProgramObjectCube);
		
		//Declaration of Matrices
		float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];
        float[] rotationMatrix = new float[16];
		float[] scaleMatrix = new float[16];
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		Matrix.setIdentityM(rotationMatrix, 0);
		Matrix.setIdentityM(scaleMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -4.0f);
		Matrix.setRotateM(rotationMatrix, 0, angleCube, 1.0f, 1.0f, 1.0f);
		Matrix.scaleM(scaleMatrix, 0, 0.75f, 0.75f, 0.75f);

		//Do necessary Matrix Multiplication
		//Multiplying modelViewMatrix * scaleMatrix
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, scaleMatrix, 0);
		//Multiplying modelViewMatrix * rotationMatrix
		Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);
        //Multiply modelViewMatrix and orthographicProjectionMatrix to get modelViewProjectionMatrix
        //Order of multiplication: modelViewProjectionMatrix * perspectiveProjectionMatrix * modelViewMatrix
        //As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        //Pass modelViewProjectionMatrix to vertex shader in mvpUniform variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao_cube[0]);
		
		//simillar Bind with Texture if any
		//Work with texture [A][B][U]
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, color_texture[0]);
		GLES32.glUniform1i(samplerUniform, 0);

        //Draw necessary Scence
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

        //Unbind with vao
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);

		
        requestRender();
		
		if (gbAnimation == true)
		{
			angleLight += speed;

			if (angleLight >= 360.0f)
			{
				angleLight = 0.0f;
			}
			
			angleCube -= speedCube;
			
			if(angleCube <= -360.0f) {
				angleCube = 0.0f;
			}
			
			if(incdecClearColor == 0)
			{
				redClearColor += 0.01f;
				blueClearColor += 0.01f;
				if(redClearColor >= 0.58f)
				{
					redClearColor = 0.58f;
					if(blueClearColor >= 0.82f)
					{
						blueClearColor = 0.82f;
						//android.os.SystemClock.sleep(1000);
						incdecClearColor = 1;
					}
				}
			}//V
			
			if(incdecClearColor == 1)
			{
				redClearColor -= 0.01f;
				blueClearColor -= 0.01f;
				if(redClearColor <= 0.29f)
				{
					redClearColor = 0.29f;
					if(blueClearColor <= 0.50f)
					{
						blueClearColor = 0.50f;
						incdecClearColor = 2;
					}
				}
			}//I
			
			if(incdecClearColor == 2)
			{
				redClearColor -= 0.01f;
				blueClearColor += 0.01f;
				if(redClearColor <= 0.0f)
				{
					redClearColor = 0.0f;
					if(blueClearColor >= 1.0f)
					{
						blueClearColor = 1.0f;
						incdecClearColor = 3;
					}
				}
			}//B
			
			if(incdecClearColor == 3)
			{
				greenClearColor += 0.01f;
				blueClearColor -= 0.01f;
				if(greenClearColor >= 1.0f)
				{
					greenClearColor = 1.0f;
					if(blueClearColor <= 0.0f)
					{
						blueClearColor = 0.0f;
						incdecClearColor = 4;
					}
				}
			}//G
			
			if(incdecClearColor == 4)
			{
				redClearColor += 0.01f;
				if(redClearColor >= 1.0f)
				{
					redClearColor = 1.0f;
					incdecClearColor = 5;
				}
			}//Y
			
			if(incdecClearColor == 5)
			{
				greenClearColor -= 0.01f;
				if(greenClearColor <= 0.49f)
				{
					greenClearColor = 0.49f;
					incdecClearColor = 6;
				}
			}//O
			
			if(incdecClearColor == 6)
			{
				greenClearColor -= 0.01f;
				if(greenClearColor <= 0.0f)
				{
					greenClearColor = 0.0f;
					incdecClearColor = 7;
				}
			}//R
			
			if(incdecClearColor == 7)
			{
				redClearColor -= 0.01f;
				blueClearColor += 0.01f;
				if(redClearColor <= 0.58f)
				{
					redClearColor = 0.58f;
					if(blueClearColor >= 0.82f)
					{
						blueClearColor = 0.82f;
						incdecClearColor = 2;
					}
				}
			}//V
		}
    }
	
	private void uninitialize() {
		
		// destroy vao
        if(vao_sphere[0] != 0)
        {
            GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
            vao_sphere[0]=0;
        }
        
        // destroy position vbo
        if(vbo_sphere_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
            vbo_sphere_position[0]=0;
        }
        
        // destroy normal vbo
        if(vbo_sphere_normal[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
            vbo_sphere_normal[0]=0;
        }
        
        // destroy element vbo
        if(vbo_sphere_element[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
            vbo_sphere_element[0]=0;
        }
		
		if (shaderProgramObject != 0)
		{
			int[] shaderCount= new int[1];
			int shaderNumber;
			GLES32.glUseProgram(shaderProgramObject);

			//Ask Program: How many Programs are attched to you?
			GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_ATTACHED_SHADERS, shaderCount, 0);
			int[] shaders = new int[shaderCount[0]];
			GLES32.glGetAttachedShaders(shaderProgramObject, shaderCount[0], shaderCount, 0, shaders, 0);
			for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++)
			{
				//Detach everyone
				GLES32.glDetachShader(shaderProgramObject, shaders[shaderNumber]);

				//Delete The Detached Shaders
				shaders[shaderNumber] = 0;
			}
			GLES32.glDeleteProgram(shaderProgramObject);
			shaderProgramObject = 0;
			GLES32.glUseProgram(0);
		}
    }
}
