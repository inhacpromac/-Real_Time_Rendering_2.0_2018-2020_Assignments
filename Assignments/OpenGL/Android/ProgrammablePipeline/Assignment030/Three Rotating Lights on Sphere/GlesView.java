package com.inhacpromac.three_rotating_lights_on_sphere;

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
	
	private boolean gbAnimation = false;
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
	private int laZeroUniform = 0;
	private int ldZeroUniform = 0;
	private int lsZeroUniform = 0;
	private int laOneUniform = 0;
	private int ldOneUniform = 0;
	private int lsOneUniform = 0;
	private int laTwoUniform = 0;
	private int ldTwoUniform = 0;
	private int lsTwoUniform = 0;
    private int kaUniform = 0;
    private int kdUniform = 0;
    private int ksUniform = 0;
    private int materialShininessUniform = 0;
    private int lightZeroPositionUniform = 0;
	private int lightOnePositionUniform = 0;
	private int lightTwoPositionUniform = 0;
    private int lightingEnabledUniform = 0;
	private int numVertices;
	private int numElements;
	private float angleOfLightZero = 0.0f;
	private float angleOfLightOne = 0.0f;
	private float angleOfLightTwo = 0.0f;
	private float speed = 1.0f;
	
	private float[] lightZeroAmbient = { 0.0f, 0.0f, 0.0f, 0.0f };
	private float[] lightZeroDiffuse = { 1.0f, 0.0f, 0.0f, 0.0f };
	private float[] lightZeroSpecular = { 1.0f, 0.0f, 0.0f, 0.0f };
	private float[] lightZeroPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

	private float[] lightOneAmbient = { 0.0f, 0.0f, 0.0f, 0.0f };
	private float[] lightOneDiffuse = { 0.0f, 1.0f, 0.0f, 0.0f };
	private float[] lightOneSpecular = { 0.0f, 1.0f, 0.0f, 0.0f };
	private float[] lightOnePosition = { 0.0f, 0.0f, 0.0f, 0.0f };

	private float[] lightTwoAmbient = { 0.0f, 0.0f, 0.0f, 0.0f };
	private float[] lightTwoDiffuse = { 0.0f, 0.0f, 1.0f, 0.0f };
	private float[] lightTwoSpecular = { 0.0f, 0.0f, 1.0f, 0.0f };
	private float[] lightTwoPosition = { 0.0f, 0.0f, 0.0f, 0.0f };

	private float[] materialAmbient = { 0.0f, 0.0f, 0.0f, 0.0f };
	private float[] materialDiffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	private float[] materialSpecular = { 1.0f, 1.0f, 1.0f, 0.0f };
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
			"in vec3 vNormal;" +
			"uniform mat4 u_m_matrix;" +
			"uniform mat4 u_v_matrix;" +
			"uniform mat4 u_p_matrix;" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec4 u_light_position_zero;" +
			"uniform vec4 u_light_position_one;" +
			"uniform vec4 u_light_position_two;" +
			"out vec3 tNorm;" +
			"out vec3 light_direction_zero;" +
			"out vec3 light_direction_one;" +
			"out vec3 light_direction_two;" +
			"out vec3 viewer_vector;" +
			"void main(void)" +
			"{" +
			"   if(u_lkeypressed == 1)" +
			"   {" +
			"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
			"       tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" +
			"       light_direction_zero = vec3(u_light_position_zero - eye_coordinates);" +
			"       light_direction_one = vec3(u_light_position_one - eye_coordinates);" +
			"       light_direction_two = vec3(u_light_position_two - eye_coordinates);" +
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
			"in vec3 light_direction_zero;" +
			"in vec3 light_direction_one;" +
			"in vec3 light_direction_two;" +
			"in vec3 viewer_vector;" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec3 u_la_zero;" +
			"uniform vec3 u_ld_zero;" +
			"uniform vec3 u_ls_zero;" +
			"uniform vec3 u_la_one;" +
			"uniform vec3 u_ld_one;" +
			"uniform vec3 u_ls_one;" +
			"uniform vec3 u_la_two;" +
			"uniform vec3 u_ld_two;" +
			"uniform vec3 u_ls_two;" +
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
			"       vec3 normalized_viewer_vector = normalize(viewer_vector);" +
			"		\n" +
			"       vec3 normalized_light_direction_zero = normalize(light_direction_zero);" +
			"       float tn_dot_ld_zero = max(dot(normalized_light_direction_zero, normalized_tNorm), 0.0);" +
			"       vec3 ambient_zero = u_la_zero * u_ka;" +
			"       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" +
			"       vec3 reflection_vector_zero = reflect(-normalized_light_direction_zero, normalized_tNorm);" +
			"       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, normalized_light_direction_zero), 0.0), u_material_shininess);" +
			"		\n" +
			"       vec3 normalized_light_direction_one = normalize(light_direction_one);" +
			"       float tn_dot_ld_one = max(dot(normalized_light_direction_one, normalized_tNorm), 0.0);" +
			"       vec3 ambient_one = u_la_one * u_ka;" +
			"       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" +
			"       vec3 reflection_vector_one = reflect(-normalized_light_direction_one, normalized_tNorm);" +
			"       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, normalized_light_direction_one), 0.0), u_material_shininess);" +
			"		\n" +
			"       vec3 normalized_light_direction_two = normalize(light_direction_two);" +
			"       float tn_dot_ld_two = max(dot(normalized_light_direction_two, normalized_tNorm), 0.0);" +
			"       vec3 ambient_two = u_la_two * u_ka;" +
			"       vec3 diffuse_two = u_ld_two * u_kd * tn_dot_ld_two;" +
			"       vec3 reflection_vector_two = reflect(-normalized_light_direction_two, normalized_tNorm);" +
			"       vec3 specular_two = u_ls_two * u_ks * pow(max(dot(reflection_vector_two, normalized_light_direction_two), 0.0), u_material_shininess);" +
			"		\n" +
			"       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one + ambient_two + diffuse_two + specular_two;" +
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
				szInfoLog =  GLES32.glGetProgramInfoLog(shaderProgramObject);
				System.out.println("INHACPROMAC | Shader Program Link Log: " + szInfoLog);
				uninitialize();
				System.exit(1);
			}
		}
		
		//postLinking retriving uniform location
		mUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_m_matrix");
		vUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_v_matrix");
		pUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_p_matrix");
		laZeroUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la_zero");
		ldZeroUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld_zero");
		lsZeroUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls_zero");
		laOneUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la_one");
		ldOneUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld_one");
		lsOneUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls_one");
		laTwoUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_la_two");
		ldTwoUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ld_two");
		lsTwoUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ls_two");
		kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ka");
		kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_kd");
		ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_ks");
		materialShininessUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_material_shininess");
		lightZeroPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position_zero");
		lightOnePositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position_one");
		lightTwoPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "u_light_position_two");
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
		
		if(gbLighting == true)
		{
			GLES32.glUniform1i(lightingEnabledUniform, 1);

			GLES32.glUniform3fv(laZeroUniform, 1, lightZeroAmbient, 0);
			GLES32.glUniform3fv(ldZeroUniform, 1, lightZeroDiffuse, 0);
			GLES32.glUniform3fv(lsZeroUniform, 1, lightZeroSpecular, 0);

			GLES32.glUniform3fv(laOneUniform, 1, lightOneAmbient, 0);
			GLES32.glUniform3fv(ldOneUniform, 1, lightOneDiffuse, 0);
			GLES32.glUniform3fv(lsOneUniform, 1, lightOneSpecular, 0);

			GLES32.glUniform3fv(laTwoUniform, 1, lightTwoAmbient, 0);
			GLES32.glUniform3fv(ldTwoUniform, 1, lightTwoDiffuse, 0);
			GLES32.glUniform3fv(lsTwoUniform, 1, lightTwoSpecular, 0);

			GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
			GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
			GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);

			GLES32.glUniform1f(materialShininessUniform, materialShininess);

			GLES32.glUniform4fv(lightZeroPositionUniform, 1, lightZeroPosition, 0);
			GLES32.glUniform4fv(lightOnePositionUniform, 1, lightOnePosition, 0);
			GLES32.glUniform4fv(lightTwoPositionUniform, 1, lightTwoPosition, 0);
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

        if (gbAnimation == true)
        {
        	angleOfLightZero += speed;
			angleOfLightOne += speed;
			angleOfLightTwo += speed;

			if (angleOfLightZero >= 360.0f)
			{
				angleOfLightZero = 0.0f;
			}
			lightZeroPosition[0] = 0.0f;
			lightZeroPosition[1] = (float)Math.sin(angleOfLightZero * Math.PI / 180) * 100.0f;
			lightZeroPosition[2] = (float)Math.cos(angleOfLightZero * Math.PI / 180) * 100.0f - 5.0f;

			if (angleOfLightOne >= 360.0f)
			{
				angleOfLightOne = 0.0f;
			}
			lightOnePosition[0] = (float)Math.sin(angleOfLightOne * Math.PI / 180) * 100.0f;
			lightOnePosition[1] = 0.0f;
			lightOnePosition[2] = (float)Math.cos(angleOfLightOne * Math.PI / 180) * 100.0f - 5.0f;

			if (angleOfLightTwo >= 360.0f)
			{
				angleOfLightTwo = 0.0f;
			}
			lightTwoPosition[0] = (float)Math.cos(angleOfLightTwo * Math.PI / 180) * 100.0f -5.0f;
			lightTwoPosition[1] = (float)Math.sin(angleOfLightTwo * Math.PI / 180) * 100.0f;
			lightTwoPosition[2] = 0.0f;
        }			
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