package com.inhacpromac.shape_pattern;

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
import java.nio.IntBuffer;


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
	
	//java does not have address oprator hence we give array of 1 and pass it's name as address	
	private int[]  vao = new int[1];
	private int[]  vbo_position = new int[1];
	private int[]  vbo_color = new int[1];

	private int[]  vbo_point_element = new int[1];
	private int pointElement = 0;

	private int[]  vbo_lines_element = new int[1];
	private int lineElement = 0;

	private int[]  vbo_square_element = new int[1];
	private int squareElement = 0;

	private int[]  vbo_diagonal_square_element = new int[1];
	private int diaSquareElement = 0;

	private int[]  vbo_fan_element = new int[1];
	private int fanElement = 0;

	private int[]  vao_color_square = new int[1];
	private int[]  vbo_color_square_position = new int[1];
	private int[]  vbo_color_square_color = new int[1];
	private int[]  vbo_color_square_element = new int[1];
	private int colorSquareElement = 0;
	
	private int mvpUniform;

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
			"in vec4 vPosition;" +
			"in vec4 vColor;" +
			"uniform mat4 u_mvp_matrix;" +
			"out vec4 out_color;" +
			"void main(void)" +
			"{" +
			"	gl_PointSize = 5.0;" +
			"   gl_Position = u_mvp_matrix * vPosition;" +
			"	out_color = vColor;" +
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
			"in vec4 out_color;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"	FragColor = out_color;" +
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
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		///////////////////////////////////////////////////////////////////////////////////////////////
		final float[] vertices = new float[]
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

		final float[] squareColor = new float[]
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
		};

		final float[] colorVertices = new float[48];
		for (int i = 0; i < 48; i++)
		{
			colorVertices[i] = 1.0f;
		}
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);
		//Position of vertices
		GLES32.glGenBuffers(1, vbo_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(vertices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        FloatBuffer positionBufferFloat = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBufferFloat.put(vertices);
		//5: Set the array at "0TH" position of the buffer.
        positionBufferFloat.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, vertices.length * 4, positionBufferFloat, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Points
		final int[] pointIndices = new int[16];
		pointElement = 16;
		for (int iItr = 0; iItr < 16; iItr++)
		{
			pointIndices[iItr] = iItr;
		}
		
		GLES32.glGenBuffers(1, vbo_point_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_point_element[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(pointIndices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the int type buffer and Convert our ByteBuffer in IntBuffer.
        IntBuffer positionBuffer = byteBuffer.asIntBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(pointIndices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, pointIndices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Lines
		final int[] lineIndices = new int[30];
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

		lineElement = iIndex;
		GLES32.glGenBuffers(1, vbo_lines_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_lines_element[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(lineIndices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the int type buffer and Convert our ByteBuffer in IntBuffer.
        positionBuffer = byteBuffer.asIntBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(lineIndices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, lineIndices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Squares
		final int[] squareIndices = new int[30];
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

		squareElement = iIndex;
		GLES32.glGenBuffers(1, vbo_square_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_square_element[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(squareIndices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the int type buffer and Convert our ByteBuffer in IntBuffer.
        positionBuffer = byteBuffer.asIntBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(squareIndices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, squareIndices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Square with Diagonal lines
		final int[] diagonalSquareIndices = new int[30];
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

		diaSquareElement = iIndex;
		GLES32.glGenBuffers(1, vbo_diagonal_square_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_diagonal_square_element[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(diagonalSquareIndices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the int type buffer and Convert our ByteBuffer in IntBuffer.
        positionBuffer = byteBuffer.asIntBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(diagonalSquareIndices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, diagonalSquareIndices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Fan
		final int[] fanIndices = new int[30];
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

		fanElement = iIndex;
		GLES32.glGenBuffers(1, vbo_fan_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_fan_element[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(fanIndices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the int type buffer and Convert our ByteBuffer in IntBuffer.
        positionBuffer = byteBuffer.asIntBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(fanIndices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, fanIndices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Color
		GLES32.glGenBuffers(1, vbo_color, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(colorVertices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        positionBufferFloat = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBufferFloat.put(colorVertices);
		//5: Set the array at "0TH" position of the buffer.
        positionBufferFloat.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, colorVertices.length * 4, positionBufferFloat, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
		
		//Colored Squares
		final float[] coloredVertices = new float[]
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

		final float[] coloredSquareColor = new float[]
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

		final int[] colorSquareIndices = new int[300];
		iIndex = 0;
		colorSquareIndices[iIndex++] = 16;
		colorSquareIndices[iIndex++] = 20;
		colorSquareIndices[iIndex++] = 21;

		colorSquareIndices[iIndex++] = 21;
		colorSquareIndices[iIndex++] = 16;
		colorSquareIndices[iIndex++] = 17;

		colorSquareIndices[iIndex++] = 17;
		colorSquareIndices[iIndex++] = 21;
		colorSquareIndices[iIndex++] = 22;

		colorSquareIndices[iIndex++] = 22;
		colorSquareIndices[iIndex++] = 17;
		colorSquareIndices[iIndex++] = 18;

		colorSquareIndices[iIndex++] = 18;
		colorSquareIndices[iIndex++] = 22;
		colorSquareIndices[iIndex++] = 23;

		colorSquareIndices[iIndex++] = 23;
		colorSquareIndices[iIndex++] = 18;
		colorSquareIndices[iIndex++] = 19;

		colorSquareIndices[iIndex++] = 15;
		colorSquareIndices[iIndex++] = 11;
		colorSquareIndices[iIndex++] = 10;

		colorSquareIndices[iIndex++] = 10;
		colorSquareIndices[iIndex++] = 15;
		colorSquareIndices[iIndex++] = 14;

		colorSquareIndices[iIndex++] = 14;
		colorSquareIndices[iIndex++] = 10;
		colorSquareIndices[iIndex++] = 9;

		colorSquareIndices[iIndex++] = 9;
		colorSquareIndices[iIndex++] = 14;
		colorSquareIndices[iIndex++] = 13;

		colorSquareIndices[iIndex++] = 13;
		colorSquareIndices[iIndex++] = 9;
		colorSquareIndices[iIndex++] = 8;

		colorSquareIndices[iIndex++] = 8;
		colorSquareIndices[iIndex++] = 13;
		colorSquareIndices[iIndex++] = 12;

		colorSquareIndices[iIndex++] = 4;
		colorSquareIndices[iIndex++] = 0;
		colorSquareIndices[iIndex++] = 1;

		colorSquareIndices[iIndex++] = 1;
		colorSquareIndices[iIndex++] = 4;
		colorSquareIndices[iIndex++] = 5;

		colorSquareIndices[iIndex++] = 5;
		colorSquareIndices[iIndex++] = 1;
		colorSquareIndices[iIndex++] = 2;

		colorSquareIndices[iIndex++] = 2;
		colorSquareIndices[iIndex++] = 5;
		colorSquareIndices[iIndex++] = 6;

		colorSquareIndices[iIndex++] = 6;
		colorSquareIndices[iIndex++] = 2;
		colorSquareIndices[iIndex++] = 3;

		colorSquareIndices[iIndex++] = 3;
		colorSquareIndices[iIndex++] = 6;
		colorSquareIndices[iIndex++] = 7;


		colorSquareElement = iIndex;

		//SquaresWithColorVAO
		GLES32.glGenVertexArrays(1, vao_color_square, 0);
		GLES32.glBindVertexArray(vao_color_square[0]);
		
		GLES32.glGenBuffers(1, vbo_color_square_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_square_position[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(coloredVertices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        positionBufferFloat = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBufferFloat.put(coloredVertices);
		//5: Set the array at "0TH" position of the buffer.
        positionBufferFloat.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, coloredVertices.length * 4, positionBufferFloat, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glGenBuffers(1, vbo_color_square_color, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_square_color[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(coloredSquareColor.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        positionBufferFloat = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBufferFloat.put(coloredSquareColor);
		//5: Set the array at "0TH" position of the buffer.
        positionBufferFloat.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, coloredSquareColor.length * 4, positionBufferFloat, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_COLOR);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glGenBuffers(1, vbo_color_square_element, 0);
		GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_color_square_element[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		byteBuffer = ByteBuffer.allocateDirect(colorSquareIndices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        positionBuffer = byteBuffer.asIntBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(colorSquareIndices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER, colorSquareIndices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

		GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		
		Matrix.setIdentityM(perspectiveProjectionMatrix,0);
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
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];
		float[] rotationMatrix = new float[16];

		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, -3.0f, 1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_point_element[0]);
		GLES32.glDrawElements(GLES32.GL_POINTS, pointElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 0.0f, 1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_lines_element[0]);
		GLES32.glDrawElements(GLES32.GL_LINES, lineElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 3.0f, 1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_square_element[0]);
		GLES32.glDrawElements(GLES32.GL_LINES, squareElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, -3.0f, -1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_diagonal_square_element[0]);
		GLES32.glDrawElements(GLES32.GL_LINES, diaSquareElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 0.0f, -1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_fan_element[0]);
		GLES32.glDrawElements(GLES32.GL_LINES, fanElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 2.5f, -1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao_color_square[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_color_square_element[0]);
		GLES32.glDrawElements(GLES32.GL_TRIANGLES, colorSquareElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);
		
		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 3.0f, -1.0f, -8.0f);
		
		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture if any

        //Draw necessary Scence
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_square_element[0]);
		GLES32.glDrawElements(GLES32.GL_LINES, squareElement, GLES32.GL_UNSIGNED_INT, 0);

        //Unbind with vao
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);
		
        requestRender();
    }
	
	private void uninitialize() {
		
		// if(vao_square[0] != 0) 
		// {
            // GLES32.glDeleteVertexArrays(1, vao_square, 0);
            // vao_square[0] = 0;
        // }
		
		
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
