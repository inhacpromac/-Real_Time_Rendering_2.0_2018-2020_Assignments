package com.inhacpromac.per_fragment;

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

//implements OnGestureListener, OnDoubleTapListener :means
//all events in this class are manditory to use even though it is empty
//implementing inner class of GLSurfaceView, Renderer
public class GlesView extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

	private final Context context;
    private GestureDetector gestureDetector;
	
	//private boolean gbAnimation = false;
	private boolean gbLighting = false;
	
	//java does not have Uint nor GLuint
	private int vertexShaderObject;
    private int fragmentShaderObject;
    private int shaderProgramObject;
	
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
	
	private float[] lightAmbient = { 0.0f,0.0f,0.0f,1.0f };
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
		// if(gbAnimation == false)
		// {
			// gbAnimation = true;
		// }
		// else
		// {
			// gbAnimation = false;
		// }
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
        GLES32.glGenBuffers(1,vbo_sphere_element,0);
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
        float[] modelMatrix = new float[16];
        float[] viewMatrix = new float[16];
        float[] ProjectionMatrix = new float[16];

		//Initialize above Matrices to identity
        Matrix.setIdentityM(modelMatrix, 0);
        Matrix.setIdentityM(viewMatrix, 0);
        Matrix.setIdentityM(ProjectionMatrix, 0);
		
		//Do necessary Trasformation
		Matrix.translateM(modelMatrix, 0, 0.0f, 0.0f, -3.0f);

		//Do necessary Matrix Multiplication
        Matrix.multiplyMM(ProjectionMatrix, 0, perspectiveProjectionMatrix, 0, ProjectionMatrix, 0);
		
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
        GLES32.glUniformMatrix4fv(pUniform, 1, false, ProjectionMatrix, 0);

        //Bind with vao
        GLES32.glBindVertexArray(vao_sphere[0]);
        
		//Draw necessary Scence
        // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
        GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
        GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
		
		//simillar Bind with Texture if any


        //Unbind with vao
        GLES32.glBindVertexArray(0);

        GLES32.glUseProgram(0);
		
        requestRender();
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
