package com.inhacpromac.tessellation_shader;

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

//implements OnGestureListener, OnDoubleTapListener :means
//all events in this class are manditory to use even though it is empty
//implementing inner class of GLSurfaceView, Renderer
public class GlesView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
    private GestureDetector gestureDetector;
	
	//java does not have Uint nor GLuint
	private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
    private int tessellationControlShaderObject;
    private int tessellationEvaluationShaderObject;
	
	//java does not have address oprator hence we give array of 1 and pass it's name as address
    private int[] vao = new int[1];
    private int[] vbo = new int[1];
	final float[] color = new float[]
	{
		1.0f, 1.0f, 0.0f, 1.0f
	};
	private int mUniform;
	private int vUniform;
	private int pUniform;
	private int numberOfSegmentsUniform;
	private int numberOfLineSegments;
	private int numberOfStripsUniform;
	private int lineColorUniform;

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
		numberOfLineSegments++;

			if (numberOfLineSegments >= 50)
			{
				numberOfLineSegments = 50;
			}
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
		numberOfLineSegments--;

			if (numberOfLineSegments <= 1)
			{
				numberOfLineSegments = 1;
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
			"in vec2 vPosition;" +
			"void main(void)" +
			"{" +
			"   gl_Position = vec4(vPosition, 0.0, 1.0);" +
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
		///////////////////////////////////////////////////////////////////////////////////////////////
		//TESSELLATION control shader
		//Define Tessellation Shader Object
		tessellationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);
		
		//Write Tessellation Shader Object
		final String tessellationControlShaderSourceCode = String.format
		(	
			"#version 320 es" +
			"\n" +
			"precision highp float;" +
			"layout(vertices=4)out;" +
			"uniform mediump int numberOfSegments;" +
			"uniform mediump int numberOfStrips;" +
			"void main(void)" +
			"{" +
			"	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;" +
			"	gl_TessLevelOuter[0] = float(numberOfStrips);" +
			"	gl_TessLevelOuter[1] = float(numberOfSegments);" +
			"}"
		);
		//Specifying Above code to the tessellationControlShaderObject
		GLES32.glShaderSource(tessellationControlShaderObject, tessellationControlShaderSourceCode);
		//Compile the Tessellation Shader
		GLES32.glCompileShader(tessellationControlShaderObject);
		//Error Checking for Tessellation Shader Compiling
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(tessellationControlShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tessellationControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(tessellationControlShaderObject);
				System.out.println("INHACPROMAC | Tessellation Control Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		
		
		//TESSELLATION evaluation shader
		//Define Tessellation Shader Object
		tessellationEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);
		
		//Write Tessellation Shader Object
		final String tessellationEvaluationShaderSourceCode = String.format
		(	
			"#version 320 es" +
			"\n" +
			"precision highp float;" +
			"layout(isolines)in;" +
			"uniform mat4 u_m_matrix;" +
			"uniform mat4 u_v_matrix;" +
			"uniform mat4 u_p_matrix;" +
			"void main(void)" +
			"{" +
			"   /*Bernstein equation*/" +
			"	float u = gl_TessCoord.x;" +
			"	vec3 p0 = gl_in[0].gl_Position.xyz;" +
			"	vec3 p1 = gl_in[1].gl_Position.xyz;" +
			"	vec3 p2 = gl_in[2].gl_Position.xyz;" +
			"	vec3 p3 = gl_in[3].gl_Position.xyz;" +
			"	float u1 = (1.0 - u);" +
			"	float u2 = u * u;" +
			"	float b3 = u2 * u;" +
			"	float b2 = 3.0 * u2 * u1;" +
			"	float b1 = 3.0 * u * u1 * u1;" +
			"	float b0 = u1 * u1 * u1;" +
			"	vec3 p = p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;" +
			"	gl_Position = u_p_matrix * u_v_matrix * u_m_matrix * vec4(p, 1.0);" +
			"}"
		);
		//Specifying Above code to the tessellationEvaluationShaderObject
		GLES32.glShaderSource(tessellationEvaluationShaderObject, tessellationEvaluationShaderSourceCode);
		//Compile the Tessellation Shader
		GLES32.glCompileShader(tessellationEvaluationShaderObject);
		//Error Checking for Tessellation Shader Compiling
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(tessellationEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(tessellationEvaluationShaderObject, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(tessellationEvaluationShaderObject);
				System.out.println("INHACPROMAC | Tessellation Evaluation Shader Compile Log: " + szInfoLog);
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
			"uniform vec4 lineColor;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"	FragColor = lineColor;" +
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
		//Attach Tessellation Control Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObject, tessellationControlShaderObject);
		//Attach Tessellation Evaluation Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
		//Attach Fragment Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);
		//preLinking Binding to Vertex Attributes
		GLES32.glBindAttribLocation(shaderProgramObject, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
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
				System.exit(0);
			}
		}
		//postLinking retriving uniform location
		mUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_m_matrix");
		vUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_v_matrix");
		pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_p_matrix");
		numberOfSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfSegments");
		numberOfStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "numberOfStrips");
		lineColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "lineColor");
		///////////////////////////////////////////////////////////////////////////////////////////////
		
		final float[] vertices = new float[]
		{
			-1.0f, -1.0f, -0.5f, 1.0f, 0.5f, -1.0f, 1.0f, 1.0f
		};

		//Create vao
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);

		GLES32.glGenBuffers(1, vbo, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(vertices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, vertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
		
		GLES32.glLineWidth(4.0f);
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		numberOfLineSegments = 1; // initially 1 straignt line
		
		Matrix.setIdentityM(perspectiveProjectionMatrix, 0);
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

		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(projectionMatrix, 0);
        Matrix.setIdentityM(translationMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);

		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelMatrix, 0, translationMatrix, 0, modelMatrix, 0);
        Matrix.multiplyMM(projectionMatrix, 0, perspectiveProjectionMatrix, 0, projectionMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mUniform, 1, false, modelMatrix, 0);
        GLES32.glUniformMatrix4fv(vUniform, 1, false, viewMatrix, 0);
        GLES32.glUniformMatrix4fv(pUniform, 1, false, projectionMatrix, 0);
		
		// send other uniforms
		GLES32.glUniform1i(numberOfSegmentsUniform, numberOfLineSegments);
		GLES32.glUniform1i(numberOfStripsUniform, 1);
		GLES32.glUniform4fv(lineColorUniform, 1, color, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
		GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES, 4);
        GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);

        //Unbind with vao
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);
		
        requestRender();
    }
	
	private void uninitialize()
	{
		if(vbo[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo, 0);
            vbo[0] = 0;
        }
		
		if(vao[0] != 0) 
		{
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
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
