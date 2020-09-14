package com.inhacpromac.interleaved;

//added by inhacpromac
//for context
import android.content.Context;


//for such a view which will give OpenGL supported surface
import android.opengl.GLSurfaceView;
import android.opengl.GLES32;

import android.opengl.GLUtils;

import android.graphics.BitmapFactory;
import android.graphics.Bitmap;

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

//implements OnGestureListener, OnDoubleTapListener :means
//all events in this class are manditory to use even though it is empty
//implementing inner class of GLSurfaceView, Renderer
public class GlesView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
    private GestureDetector gestureDetector;
	
	private boolean gbAnimation = false;
	private boolean gbLighting = false;
	
	//java does not have Uint nor GLuint
	private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
	
	//java does not have address oprator hence we give array of 1 and pass it's name as address
	private int[] vao_interleaved = new int[1];
    private int[] vbo_interleaved = new int[1];
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
	
	private int samplerUniform;
	private int[] texture_marble = new int[1];
	
	private float angleCube = 0.0f;
	private float speed = 1.0f;
	
	private float[] lightAmbient = { 0.25f,0.25f,0.25f,1.0f };
    private float[] lightDiffuse = { 1.0f,1.0f,1.0f,1.0f };
    private float[] lightSpecular = { 1.0f,1.0f,1.0f,1.0f };
    private float[] lightPosition = { 100.0f,100.0f,100.0f,1.0f };

    private float[] materialAmbient = { 0.0f,0.0f,0.0f,1.0f };
    private float[] materialDiffuse = { 1.0f,1.0f,1.0f,1.0f };
    private float[] materialSpecular = { 1.0f,1.0f,1.0f,1.0f };
    private float materialShininess = 128.0f; //50.0f

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
		}
		else
		{
			gbAnimation = false;
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
		if(gbLighting == false)
		{
			gbLighting = true;
		}
		else
		{
			gbLighting = false;
		}
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
        resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 unused) {
        display();
    }
	
	//Our custom methods
	private void initialize() {
		
		///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
		//Define Vertex Shader Object
		vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		//Write Vertex Shader Object
		final String vertexShaderSourceCode = String.format
		(	
			"#version 320 es" +
			"\n" +
			"in vec4 vPosition;" +
			"in vec4 vColor;" +
			"in vec3 vNormal;" +
			"in vec2 vTexCoord;" +
			"\n" +
			"uniform mat4 u_m_matrix;" +
			"uniform mat4 u_v_matrix;" +
			"uniform mat4 u_p_matrix;" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec4 u_light_position;" +
			"\n" +
			"out vec3 tNorm;" +
			"out vec2 out_texcoord;" +
			"out vec3 light_direction;" +
			"out vec3 viewer_vector;" +
			"out vec4 out_color;" +
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
			"	out_color = vColor;" +
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
			"in vec3 tNorm;" +
			"in vec3 light_direction;" +
			"in vec3 viewer_vector;" +
			"in vec2 out_texcoord;" +
			"in vec4 out_color;" +
			"\n" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec3 u_la;" +
			"uniform vec3 u_ld;" +
			"uniform vec3 u_ls;" +
			"uniform vec3 u_ka;" +
			"uniform vec3 u_kd;" +
			"uniform vec3 u_ks;" +
			"uniform float u_material_shininess;" +
			"uniform sampler2D u_sampler;" +
			"\n" +
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
			"	FragColor = texture(u_sampler, out_texcoord) * out_color * vec4(phong_ads_light, 1.0);" +
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
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_COLOR, "vColor");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");
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
		samplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_sampler");
		///////////////////////////////////////////////////////////////////////////////////////////////
		//Below have this order for every vertex at (x,y,z)
		//x, y, z, cx, cy, cz, nx, ny, nz, tx, ty
		final float[] interleaved_data = new float[]
		{
			// Top face
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

			// Bottom face
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,

			// Front face
			1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

			// Back face
			1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

			// Right face
			1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

			// Left face
			-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		
		//Create vao_interleaved
		GLES32.glGenVertexArrays(1, vao_interleaved, 0);
		GLES32.glBindVertexArray(vao_interleaved[0]);
		//Position
		GLES32.glGenBuffers(1, vbo_interleaved, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_interleaved[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(interleaved_data.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(interleaved_data);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 24 * 11 * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 11 * 4, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);
		
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 11 * 4, 3 * 4);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);
		
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 11 * 4, 6 * 4);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_NORMAL);
		
		
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 11 * 4, 9 * 4);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);
		
		texture_marble[0] = loadGLTexture(R.raw.marble);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
    }
	
	private int loadGLTexture(int imageResourceID) {
		
		int[] texture = new int[1];
		
		BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;

        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageResourceID, options);
		
		GLES32.glGenTextures(1, texture, 0);
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture[0]);
		GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
        GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

        GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0);

        //Generate mipmapped texture
        GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);

        //Unbind the texture
        GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

        return texture[0];
    }
	
	private void resize(int width, int height) {
		if (height == 0)
		{
			height = 1;
		}
        GLES32.glViewport(0, 0, width, height);
		Matrix.perspectiveM(perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

		//Declaration of Matrices
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];
        float[] projectionMatrix = new float[16];
        float[] translationMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] scaleMatrix = new float[16];

		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        Matrix.setIdentityM(translationMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(scaleMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -6.0f);
		Matrix.setRotateM(rotationMatrix, 0, angleCube, 1.0f, 1.0f, 1.0f);
		Matrix.scaleM(scaleMatrix, 0, 0.75f, 0.75f, 0.75f);

		//Do necessary Matrix Multiplication
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, translationMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, scaleMatrix, 0);
		Matrix.multiplyMM(modelMatrix, 0, modelMatrix, 0, rotationMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, perspectiveProjectionMatrix, 0, projectionMatrix, 0);
		
		if(gbLighting)
		{
            GLES32.glUniform1i(lightingEnabledUniform, 1);
            GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
			GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
			GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);
			GLES32.glUniform1f(materialShininessUniform, materialShininess);
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
        GLES32.glBindVertexArray(vao_interleaved[0]);
		
		//simillar Bind with Texture if any
		//Work with texture [A][B][U]
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_marble[0]);
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
		
		if (gbAnimation == true)
		{
			angleCube -= speed;

			if(angleCube <= -360.0f) 
			{
				angleCube = 0.0f;
			}
		}
		
        requestRender();
    }
	
	private void uninitialize() {
		
		if(vao_interleaved[0] != 0) 
		{
            GLES32.glDeleteVertexArrays(1, vao_interleaved, 0);
            vao_interleaved[0] = 0;
        }
		
		if(vbo_interleaved[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_interleaved, 0);
            vbo_interleaved[0] = 0;
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
