package com.inhacpromac.per_vertex_per_fragment_toggle;

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
	private boolean gbToggle = true;	//true = pervertex
	
	//java does not have Uint nor GLuint
	private int vertexShaderObjectPerVertex;
    private int fragmentShaderObjectPerVertex;
    private int shaderProgramObjectPerVertex;
	private int vertexShaderObjectPerFragment;
    private int fragmentShaderObjectPerFragment;
    private int shaderProgramObjectPerFragment;
	
	//java does not have address oprator hence we give array of 1 and pass it's name as address
	private int[] vao_sphere = new int[1];
    private int[] vbo_sphere_position = new int[1];
    private int[] vbo_sphere_normal = new int[1];
    private int[] vbo_sphere_element = new int[1];
	
	private int mUniformPerVertex;
	private int vUniformPerVertex;
	private int pUniformPerVertex;
	
	private int laUniformPerVertex = 0;
	private int ldUniformPerVertex = 0;
	private int lsUniformPerVertex = 0;
	private int kaUniformPerVertex = 0;
	private int kdUniformPerVertex = 0;
	private int ksUniformPerVertex = 0;
	private int materialShininessUniformPerVertex = 0;
	private int lightPositionUniformPerVertex = 0;
	private int lightingEnabledUniformPerVertex;
	
	private int mUniformPerFragment;
	private int vUniformPerFragment;
	private int pUniformPerFragment;
	
	private int laUniformPerFragment = 0;
	private int ldUniformPerFragment = 0;
	private int lsUniformPerFragment = 0;
	private int kaUniformPerFragment = 0;
	private int kdUniformPerFragment = 0;
	private int ksUniformPerFragment = 0;
	private int materialShininessUniformPerFragment = 0;
	private int lightPositionUniformPerFragment = 0;
	private int lightingEnabledUniformPerFragment;
	
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
		if(gbToggle == false)
		{
			gbToggle = true;
		}
		else
		{
			gbToggle = false;
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
//xx//xx//xx//PerVertexInitilization//xx//xx//xx//

//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
		///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
		//Define Vertex Shader Object
		vertexShaderObjectPerVertex = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		//Write Vertex Shader Object
		final String vertexShaderSourceCodePerVertex = String.format
		(	
			"#version 320 es" +
			"\n" +
			"in vec4 vPosition;" +
			"in vec3 vNormal;" +
			"uniform mat4 u_m_matrix;" +
			"uniform mat4 u_v_matrix;" +
			"uniform mat4 u_p_matrix;" +
			"uniform mediump int u_lkeypressed;" +
			"uniform vec3 u_la;" +
			"uniform vec3 u_ld;" +
			"uniform vec3 u_ls;" +
			"uniform vec3 u_ka;" +
			"uniform vec3 u_kd;" +
			"uniform vec3 u_ks;" +
			"uniform vec4 u_light_position;" +
			"uniform float u_material_shininess;" +
			"out vec3 phong_ads_light;" +
			"void main(void)" +
			"{" +
			"   if(u_lkeypressed == 1)" +
			"   {" +
			"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
			"       vec3 tNorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" +
			"       vec3 light_direction = normalize(vec3(u_light_position - eye_coordinates));" +
			"       float tn_dot_ld = max(dot(light_direction, tNorm), 0.0);" +
			"       vec3 reflection_vector = reflect(-light_direction, tNorm);" +
			"       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
			"       vec3 ambient = u_la * u_ka;" +
			"       vec3 diffuse = u_ld * u_kd * tn_dot_ld;" +
			"       vec3 specular = u_ls * u_ks * pow(max(dot(reflection_vector, viewer_vector), 0.0), u_material_shininess);" +
			"       phong_ads_light = ambient + diffuse + specular;" +
			"   }" +
			"   else" +
			"   {" +
			"       phong_ads_light = vec3(1.0, 1.0, 1.0);" +
			"   }" +
			"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" +
			"}"
		);
		//Specifying Above code to the VertexShaderObject
		GLES32.glShaderSource(vertexShaderObjectPerVertex, vertexShaderSourceCodePerVertex);
		//Compile the Vertex Shader
		GLES32.glCompileShader(vertexShaderObjectPerVertex);
		//Error Checking for Vertex Shader Compiling
		int[]  iShaderCompileStatus = new int[1];
		int[]  iInfoLogLength = new int[1];
		String szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObjectPerVertex, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObjectPerVertex, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObjectPerVertex);
				System.out.println("INHACPROMAC | Vertex Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
		//Define Fragment Shader Object
		fragmentShaderObjectPerVertex = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		//Write Fragment Shader Object
		final String fragmentShaderSourceCodePerVertex = String.format
		(
			"#version 320 es" +
			"\n" +
			"precision highp float;" +
			"in vec3 phong_ads_light;" +
			"out vec4 FragColor;" +
			"uniform mediump int u_lkeypressed;" +
			"void main(void)" +
			"{" +
			"   if(u_lkeypressed == 1)" +
			"   {" +
			"       FragColor = vec4(phong_ads_light, 1.0);" +
			"   }" +
			"   else" +
			"   {" +
			"       FragColor = vec4(1.0, 1.0, 1.0, 1.0);" +
			"   }" +
			"}"
		);
		//Specifying Above code to the FragmentShaderObject
		GLES32.glShaderSource(fragmentShaderObjectPerVertex, fragmentShaderSourceCodePerVertex);
		//Compile the fragment Shader
		GLES32.glCompileShader(fragmentShaderObjectPerVertex);
		//Error Checking for Fragment Shader Compiling
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(fragmentShaderObjectPerVertex, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObjectPerVertex, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObjectPerVertex);
				System.out.println("INHACPROMAC | Fragment Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
		//Create Shader Program Object
		shaderProgramObjectPerVertex = GLES32.glCreateProgram();
		//Attach Vertex Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObjectPerVertex, vertexShaderObjectPerVertex);
		//Attach Fragment Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObjectPerVertex, fragmentShaderObjectPerVertex);
		//preLinking Binding to Vertex Attributes
		GLES32.glBindAttribLocation(shaderProgramObjectPerVertex, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectPerVertex, GLESMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
		//Link The Shader Program
		GLES32.glLinkProgram(shaderProgramObjectPerVertex);
		//Error Checking for Shader Program Linking
		int[] iProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetProgramiv(shaderProgramObjectPerVertex, GLES32.GL_LINK_STATUS, iProgramLinkStatus, 0);
		if (iProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectPerVertex, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				GLES32.glGetProgramInfoLog(shaderProgramObjectPerVertex);
				System.out.println("INHACPROMAC | Shader Program Link Log: " + szInfoLog);
				uninitialize();
				System.exit(1);
			}
		}
		//postLinking retriving uniform location
		mUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_m_matrix");
		vUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_v_matrix");
		pUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_p_matrix");
		laUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_la");
		ldUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_ld");
		lsUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_ls");
		kaUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_ka");
		kdUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_kd");
		ksUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_ks");
		materialShininessUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_material_shininess");
		lightPositionUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_light_position");
		lightingEnabledUniformPerVertex = GLES32.glGetUniformLocation(shaderProgramObjectPerVertex, "u_lkeypressed");
		///////////////////////////////////////////////////////////////////////////////////////////////
//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
		

//xx//xx//xx//PerFragmentInitilization//xx//xx//xx//

//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
		///////////////////////////////////////////////////////////////////////////////////////////////Vertex Shader
		//Define Vertex Shader Object
		vertexShaderObjectPerFragment = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
		//Write Vertex Shader Object
		final String vertexShaderSourceCodePerFragment = String.format
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
		GLES32.glShaderSource(vertexShaderObjectPerFragment, vertexShaderSourceCodePerFragment);
		//Compile the Vertex Shader
		GLES32.glCompileShader(vertexShaderObjectPerFragment);
		//Error Checking for Vertex Shader Compiling
		iShaderCompileStatus = new int[1];
		iInfoLogLength = new int[1];
		szInfoLog = null;
		GLES32.glGetShaderiv(vertexShaderObjectPerFragment, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(vertexShaderObjectPerFragment, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObjectPerFragment);
				System.out.println("INHACPROMAC | Vertex Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Fragment Shader
		//Define Fragment Shader Object
		fragmentShaderObjectPerFragment = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
		//Write Fragment Shader Object
		final String fragmentShaderSourceCodePerFragment = String.format
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
		GLES32.glShaderSource(fragmentShaderObjectPerFragment, fragmentShaderSourceCodePerFragment);
		//Compile the fragment Shader
		GLES32.glCompileShader(fragmentShaderObjectPerFragment);
		//Error Checking for Fragment Shader Compiling
		iShaderCompileStatus[0] = 0;
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetShaderiv(fragmentShaderObjectPerFragment, GLES32.GL_COMPILE_STATUS, iShaderCompileStatus, 0);
		if (iShaderCompileStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetShaderiv(fragmentShaderObjectPerFragment, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObjectPerFragment);
				System.out.println("INHACPROMAC | Fragment Shader Compile Log: " + szInfoLog);
				uninitialize();
				System.exit(0);
			}
		}
		///////////////////////////////////////////////////////////////////////////////////////////////Shader Program
		//Create Shader Program Object
		shaderProgramObjectPerFragment = GLES32.glCreateProgram();
		//Attach Vertex Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObjectPerFragment, vertexShaderObjectPerFragment);
		//Attach Fragment Shader to Shader Program
		GLES32.glAttachShader(shaderProgramObjectPerFragment, fragmentShaderObjectPerFragment);
		//preLinking Binding to Vertex Attributes
		GLES32.glBindAttribLocation(shaderProgramObjectPerFragment, GLESMacros.AMC_ATTRIBUTE_POSITION, "vPosition");
		GLES32.glBindAttribLocation(shaderProgramObjectPerFragment, GLESMacros.AMC_ATTRIBUTE_NORMAL, "vNormal");
		//Link The Shader Program
		GLES32.glLinkProgram(shaderProgramObjectPerFragment);
		//Error Checking for Shader Program Linking
		iProgramLinkStatus = new int[1];
		iInfoLogLength[0] = 0;
		szInfoLog = null;
		GLES32.glGetProgramiv(shaderProgramObjectPerFragment, GLES32.GL_LINK_STATUS, iProgramLinkStatus, 0);
		if (iProgramLinkStatus[0] == GLES32.GL_FALSE)
		{
			GLES32.glGetProgramiv(shaderProgramObjectPerFragment, GLES32.GL_INFO_LOG_LENGTH, iInfoLogLength, 0);
			if (iInfoLogLength[0] > 0)
			{
				GLES32.glGetProgramInfoLog(shaderProgramObjectPerFragment);
				System.out.println("INHACPROMAC | Shader Program Link Log: " + szInfoLog);
				uninitialize();
				System.exit(1);
			}
		}
		//postLinking retriving uniform location
		mUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_m_matrix");
		vUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_v_matrix");
		pUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_p_matrix");
		laUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_la");
		ldUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_ld");
		lsUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_ls");
		kaUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_ka");
		kdUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_kd");
		ksUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_ks");
		materialShininessUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_material_shininess");
		lightPositionUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_light_position");
		lightingEnabledUniformPerFragment = GLES32.glGetUniformLocation(shaderProgramObjectPerFragment, "u_lkeypressed");
		///////////////////////////////////////////////////////////////////////////////////////////////
//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
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
		if(gbToggle == true)
		{
			GLES32.glUseProgram(shaderProgramObjectPerVertex);

			//Declaration of Matrices
			float[] modelMatrixPerVertex = new float[16];
			float[] viewMatrixPerVertex = new float[16];
			float[] ProjectionMatrixPerVertex = new float[16];

			//Initialize above Matrices to identity
			Matrix.setIdentityM(modelMatrixPerVertex, 0);
			Matrix.setIdentityM(viewMatrixPerVertex, 0);
			Matrix.setIdentityM(ProjectionMatrixPerVertex, 0);
			
			//Do necessary Trasformation
			Matrix.translateM(modelMatrixPerVertex, 0, 0.0f, 0.0f, -3.0f);

			//Do necessary Matrix Multiplication
			Matrix.multiplyMM(ProjectionMatrixPerVertex, 0, perspectiveProjectionMatrix, 0, ProjectionMatrixPerVertex, 0);
			
			if(gbLighting)
			{
				GLES32.glUniform1i(lightingEnabledUniformPerVertex, 1);
				GLES32.glUniform3fv(laUniformPerVertex, 1, lightAmbient, 0);
				GLES32.glUniform3fv(ldUniformPerVertex, 1, lightDiffuse, 0);
				GLES32.glUniform3fv(lsUniformPerVertex, 1, lightSpecular, 0);
				GLES32.glUniform3fv(kaUniformPerVertex, 1, materialAmbient, 0);
				GLES32.glUniform3fv(kdUniformPerVertex, 1, materialDiffuse, 0);
				GLES32.glUniform3fv(ksUniformPerVertex, 1, materialSpecular, 0);
				GLES32.glUniform1f(materialShininessUniformPerVertex, materialShininess);
				GLES32.glUniform4fv(lightPositionUniformPerVertex, 1, lightPosition, 0);
			}
			else
			{
				GLES32.glUniform1i(lightingEnabledUniformPerVertex, 0);
			}

			GLES32.glUniformMatrix4fv(mUniformPerVertex, 1, false, modelMatrixPerVertex, 0);
			GLES32.glUniformMatrix4fv(vUniformPerVertex, 1, false, viewMatrixPerVertex, 0);
			GLES32.glUniformMatrix4fv(pUniformPerVertex, 1, false, ProjectionMatrixPerVertex, 0);

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
		}
		else if(gbToggle == false)
		{
			GLES32.glUseProgram(shaderProgramObjectPerFragment);

			//Declaration of Matrices
			float[] modelMatrixPerFragment = new float[16];
			float[] viewMatrixPerFragment = new float[16];
			float[] ProjectionMatrixPerFragment = new float[16];

			//Initialize above Matrices to identity
			Matrix.setIdentityM(modelMatrixPerFragment, 0);
			Matrix.setIdentityM(viewMatrixPerFragment, 0);
			Matrix.setIdentityM(ProjectionMatrixPerFragment, 0);
			
			//Do necessary Trasformation
			Matrix.translateM(modelMatrixPerFragment, 0, 0.0f, 0.0f, -3.0f);

			//Do necessary Matrix Multiplication
			Matrix.multiplyMM(ProjectionMatrixPerFragment, 0, perspectiveProjectionMatrix, 0, ProjectionMatrixPerFragment, 0);
			
			if(gbLighting)
			{
				GLES32.glUniform1i(lightingEnabledUniformPerFragment, 1);
				GLES32.glUniform3fv(laUniformPerFragment, 1, lightAmbient, 0);
				GLES32.glUniform3fv(ldUniformPerFragment, 1, lightDiffuse, 0);
				GLES32.glUniform3fv(lsUniformPerFragment, 1, lightSpecular, 0);
				GLES32.glUniform3fv(kaUniformPerFragment, 1, materialAmbient, 0);
				GLES32.glUniform3fv(kdUniformPerFragment, 1, materialDiffuse, 0);
				GLES32.glUniform3fv(ksUniformPerFragment, 1, materialSpecular, 0);
				GLES32.glUniform1f(materialShininessUniformPerFragment, materialShininess);
				GLES32.glUniform4fv(lightPositionUniformPerFragment, 1, lightPosition, 0);
			}
			else
			{
				GLES32.glUniform1i(lightingEnabledUniformPerFragment, 0);
			}

			GLES32.glUniformMatrix4fv(mUniformPerFragment, 1, false, modelMatrixPerFragment, 0);
			GLES32.glUniformMatrix4fv(vUniformPerFragment, 1, false, viewMatrixPerFragment, 0);
			GLES32.glUniformMatrix4fv(pUniformPerFragment, 1, false, ProjectionMatrixPerFragment, 0);

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
		
		if (shaderProgramObjectPerVertex != 0)
		{
			int[] shaderCount= new int[1];
			int shaderNumber;
			GLES32.glUseProgram(shaderProgramObjectPerVertex);

			//Ask Program: How many Programs are attched to you?
			GLES32.glGetProgramiv(shaderProgramObjectPerVertex, GLES32.GL_ATTACHED_SHADERS, shaderCount, 0);
			int[] shaders = new int[shaderCount[0]];
			GLES32.glGetAttachedShaders(shaderProgramObjectPerVertex, shaderCount[0], shaderCount, 0, shaders, 0);
			for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++)
			{
				//Detach everyone
				GLES32.glDetachShader(shaderProgramObjectPerVertex, shaders[shaderNumber]);

				//Delete The Detached Shaders
				shaders[shaderNumber] = 0;
			}
			GLES32.glDeleteProgram(shaderProgramObjectPerVertex);
			shaderProgramObjectPerVertex = 0;
			GLES32.glUseProgram(0);
		}
		
		if (shaderProgramObjectPerFragment != 0)
		{
			int[] shaderCount= new int[1];
			int shaderNumber;
			GLES32.glUseProgram(shaderProgramObjectPerFragment);

			//Ask Program: How many Programs are attched to you?
			GLES32.glGetProgramiv(shaderProgramObjectPerFragment, GLES32.GL_ATTACHED_SHADERS, shaderCount, 0);
			int[] shaders = new int[shaderCount[0]];
			GLES32.glGetAttachedShaders(shaderProgramObjectPerFragment, shaderCount[0], shaderCount, 0, shaders, 0);
			for (shaderNumber = 0; shaderNumber < shaderCount[0]; shaderNumber++)
			{
				//Detach everyone
				GLES32.glDetachShader(shaderProgramObjectPerFragment, shaders[shaderNumber]);

				//Delete The Detached Shaders
				shaders[shaderNumber] = 0;
			}
			GLES32.glDeleteProgram(shaderProgramObjectPerFragment);
			shaderProgramObjectPerFragment = 0;
			GLES32.glUseProgram(0);
		}
		
    }
}
