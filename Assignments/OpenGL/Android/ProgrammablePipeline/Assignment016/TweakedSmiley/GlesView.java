package com.inhacpromac.tweaked_smiley;

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
	
	//java does not have Uint nor GLuint
	private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
	
	//java does not have address oprator hence we give array of 1 and pass it's name as address
    private int[] vao = new int[1];
    private int[] vbo_position = new int[1];
	private int[] vbo_texture = new int[1];
	private int[] texture_smiley = new int[1];
	private int samplerUniform;
	private int mvpUniform;
	private int singleTap = 1;

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
		singleTap++;
		if(singleTap > 4)
		{
			singleTap = 1;
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
			"in vec4 vPosition;" +
			"in vec2 vTexCoord;" +
			"uniform mat4 u_mvp_matrix;" +
			"out vec2 out_texcoord;" +
			"void main(void)" +
			"{" +
			"   gl_Position = u_mvp_matrix * vPosition;" +
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
			"in vec2 out_texcoord;" +
			"uniform highp sampler2D u_sampler;" +
			"out vec4 FragColor;" +
			"void main(void)" +
			"{" +
			"	FragColor = texture(u_sampler, out_texcoord);" +
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
				System.exit(0);
			}
		}
		//postLinking retriving uniform location
		mvpUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_mvp_matrix");
		samplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_sampler");
		///////////////////////////////////////////////////////////////////////////////////////////////
		//When array is declair and not initialize then specify the size
		//but when declairing and initilizing the array dont specify the size, direcly specify the array
		final float[] rectangleVertices = new float[]
		{
			1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, 1.0f
		};

		//Create vao
		GLES32.glGenVertexArrays(1, vao, 0);
		GLES32.glBindVertexArray(vao[0]);
		//Position
		GLES32.glGenBuffers(1, vbo_position, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rectangleVertices.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(rectangleVertices);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangleVertices.length * 4, positionBuffer, GLES32.GL_STATIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_POSITION);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Texture
		GLES32.glGenBuffers(1, vbo_texture, 0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture[0]);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 8 * 4, null, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);

		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		GLES32.glBindVertexArray(0);
		
		texture_smiley[0] = loadGLTexture(R.raw.smiley);
		
		GLES32.glEnable(GLES32.GL_DEPTH_TEST);
		GLES32.glDepthFunc(GLES32.GL_LEQUAL);

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
        float[] modelViewMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelViewMatrix, 0, 0.0f, 0.0f, -6.0f);

		//Do necessary Matrix Multiplication
        //Multiply modelViewMatrix and orthographicProjectionMatrix to get modelViewProjectionMatrix
        //Order of multiplication: modelViewProjectionMatrix * perspectiveProjectionMatrix * modelViewMatrix
        //As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

		//Send necessary Matrix to Shader in respective Uniform
        //Pass modelViewProjectionMatrix to vertex shader in mvpUniform variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao[0]);
		
		//simillar Bind with Texture
		final float[] reactangleTexture = new float[8];
		
		if (singleTap == 1)
		{
			reactangleTexture[0] = 0.0f;
			reactangleTexture[1] = 0.5f;
			reactangleTexture[2] = 0.0f;
			reactangleTexture[3] = 0.0f;
			reactangleTexture[4] = 0.5f;
			reactangleTexture[5] = 0.0f;
			reactangleTexture[6] = 0.5f;
			reactangleTexture[7] = 0.5f;
		}
		else if (singleTap == 2)
		{
			reactangleTexture[0] = 0.0f;
			reactangleTexture[1] = 1.0f;
			reactangleTexture[2] = 0.0f;
			reactangleTexture[3] = 0.0f;
			reactangleTexture[4] = 1.0f;
			reactangleTexture[5] = 0.0f;
			reactangleTexture[6] = 1.0f;
			reactangleTexture[7] = 1.0f;
		}
		else if (singleTap == 3)
		{
			reactangleTexture[0] = 0.0f;
			reactangleTexture[1] = 2.0f;
			reactangleTexture[2] = 0.0f;
			reactangleTexture[3] = 0.0f;
			reactangleTexture[4] = 2.0f;
			reactangleTexture[5] = 0.0f;
			reactangleTexture[6] = 2.0f;
			reactangleTexture[7] = 2.0f;
		}
		else if (singleTap == 4)
		{
			reactangleTexture[0] = 0.5f;
			reactangleTexture[1] = 0.5f;
			reactangleTexture[2] = 0.5f;
			reactangleTexture[3] = 0.5f;
			reactangleTexture[4] = 0.5f;
			reactangleTexture[5] = 0.5f;
			reactangleTexture[6] = 0.5f;
			reactangleTexture[7] = 0.5f;
		}
		
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texture[0]);
		
		//Convert the array into compatible buffer such the we can pass it through to BufferData
		//1: Allocate the buffer directly from the native memory(Not from VM Memeory(Unmanaged memory)).
		ByteBuffer byteBuffer = ByteBuffer.allocateDirect(reactangleTexture.length * 4);//(ArrayLength*DatatypeSize)
		//2: Arrange the byte order of buffer in native byte order.
        byteBuffer.order(ByteOrder.nativeOrder());
		//3: Create the float type buffer and Convert our ByteBuffer in FloatBuffer.
        FloatBuffer positionBuffer = byteBuffer.asFloatBuffer();
		//4: Now, put your array into "cooked buffer".
        positionBuffer.put(reactangleTexture);
		//5: Set the array at "0TH" position of the buffer.
        positionBuffer.position(0);

		GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 8 * 4, positionBuffer, GLES32.GL_DYNAMIC_DRAW);
		GLES32.glVertexAttribPointer(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0, 2, GLES32.GL_FLOAT, false, 0, 0);
		GLES32.glEnableVertexAttribArray(GLESMacros.AMC_ATTRIBUTE_TEXCOORD0);
		GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
		
		//Work with texture [A][B][U]
		GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
		GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_smiley[0]);
		GLES32.glUniform1i(samplerUniform, 0);

        //Draw necessary Scence
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        //Unbind with vao
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);
		
        requestRender();
    }
	
	private void uninitialize() {
		
		if(vao[0] != 0) 
		{
            GLES32.glDeleteVertexArrays(1, vao, 0);
            vao[0] = 0;
        }
		
		if(vbo_position[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_position, 0);
            vbo_position[0] = 0;
        }
		
		if(vbo_texture[0] != 0)
        {
            GLES32.glDeleteBuffers(1, vbo_texture, 0);
            vbo_texture[0] = 0;
        }
		
		if(texture_smiley[0] != 0)
        {
            GLES32.glDeleteBuffers(1, texture_smiley, 0);
            texture_smiley[0] = 0;
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
