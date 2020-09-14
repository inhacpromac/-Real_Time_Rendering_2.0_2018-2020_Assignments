// global variables
var canvas=null;
var gl=null; // webgl context
var bFullscreen=false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros = 
// when whole 'WebGLMacros' is 'const', all inside it are automatically 'const'
{
	AMC_ATTRIBUTE_VERTEX:0,
	AMC_ATTRIBUTE_COLOR:1,
	AMC_ATTRIBUTE_NORMAL:2,
	AMC_ATTRIBUTE_TEXTURE0:3,
};

var vertexShaderObjectPerVertex;
var fragmentShaderObjectPerVertex;
var shaderProgramObjectPerVertex;
var vertexShaderObjectPerFragment;
var fragmentShaderObjectPerFragment;
var shaderProgramObjectPerFragment;

var lightAmbient=[0.0,0.0,0.0];
var lightDiffuse=[1.0,1.0,1.0];
var lightSpecular=[1.0,1.0,1.0];
var lightPosition=[100.0,100.0,100.0,1.0];

var materialAmbient= [0.0,0.0,0.0];
var materialDiffuse= [1.0,1.0,1.0];
var materialSpecular= [1.0,1.0,1.0];
var materialShininess= 50.0; //50.0

var mUniformPerVertex;
var vUniformPerVertex;
var pUniformPerVertex;

var laUniformPerVertex = 0;
var ldUniformPerVertex = 0;
var lsUniformPerVertex = 0;
var kaUniformPerVertex = 0;
var kdUniformPerVertex = 0;
var ksUniformPerVertex = 0;
var materialShininessUniformPerVertex = 0;
var lightPositionUniformPerVertex = 0;
var lightingEnabledUniformPerVertex;

var mUniformPerFragment;
var vUniformPerFragment;
var pUniformPerFragment;

var laUniformPerFragment = 0;
var ldUniformPerFragment = 0;
var lsUniformPerFragment = 0;
var kaUniformPerFragment = 0;
var kdUniformPerFragment = 0;
var ksUniformPerFragment = 0;
var materialShininessUniformPerFragment = 0;
var lightPositionUniformPerFragment = 0;
var lightingEnabledUniformPerFragment;

var gbLighting = false;
var gbToggle = true;	//true = pervertex

var sphere = null;

var perspectiveProjectionMatrix;

// To start animation : To have requestAnimationFrame() to be called "cross-browser" compatible
var requestAnimationFrame = 
	window.requestAnimationFrame || 
	window.webkitRequestAnimationFrame || 
	window.mozRequestAnimationFrame || 
	window.oRequestAnimationFrame || 
	window.msRequestAnimationFrame;

// To stop animation : To have cancelAnimationFrame() to be called "cross-browser" compatible
var cancelAnimationFrame = 
	window.cancelAnimationFrame || 
	window.webkitCancelRequestAnimationFrame || 
	window.webkitCancelAnimationFrame || 
	window.mozCancelRequestAnimationFrame || 
	window.mozCancelAnimationFrame || 
	window.oCancelRequestAnimationFrame || 
	window.oCancelAnimationFrame || 
	window.msCancelRequestAnimationFrame || 
	window.msCancelAnimationFrame;

// onload function
function main()
{
	// get <canvas> element
	canvas = document.getElementById("AMC");
	if(!canvas)
		console.log("Obtaining Canvas Failed\n");
	else
		console.log("Obtaining Canvas Succeeded\n");
	canvas_original_width=canvas.width;
	canvas_original_height=canvas.height;
	
	// register keyboard's keydown event handler
	window.addEventListener("keydown", keyDown, false);
	window.addEventListener("click", mouseDown, false);
	window.addEventListener("resize", resize, false);
	
	// initialize WebGL
	init();
	
	// start drawing here as warming-up
	resize();
	draw();
}

function toggleFullScreen()
{
	// code
	var fullscreen_element = 
		document.fullscreenElement || 
		document.webkitFullscreenElement || 
		document.mozFullScreenElement || 
		document.msFullscreenElement || 
		null;
		
	// if not fullscreen
	if(fullscreen_element == null)
	{
		if(canvas.requestFullscreen)
			canvas.requestFullscreen();
		else if(canvas.mozRequestFullScreen)
			canvas.mozRequestFullScreen();
		else if(canvas.webkitRequestFullscreen)
			canvas.webkitRequestFullscreen();
		else if(canvas.msRequestFullscreen)
			canvas.msRequestFullscreen();
		bFullscreen=true;
	}
	else // if already fullscreen
	{
		if(document.exitFullscreen)
			document.exitFullscreen();
		else if(document.mozCancelFullScreen)
			document.mozCancelFullScreen();
		else if(document.webkitExitFullscreen)
			document.webkitExitFullscreen();
		else if(document.msExitFullscreen)
			document.msExitFullscreen();
		bFullscreen=false;
	}
}

function init()
{
	// code
	// get WebGL 2.0 context
	gl=canvas.getContext("webgl2");
	if(gl==null) // failed to get context
	{
		console.log("Failed to get the rendering context for WebGL");
		return;
	}
	gl.viewportwidth = canvas.width;
	gl.viewportHeight = canvas.height;
	
	//xx//xx//xx//PerVertexInitilization//xx//xx//xx//
	//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
	// vertex shader
	var vertexShaderSourceCodePerVertex =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
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
		"}";
	
	vertexShaderObjectPerVertex=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectPerVertex,vertexShaderSourceCodePerVertex);
	gl.compileShader(vertexShaderObjectPerVertex);
	if(gl.getShaderParameter(vertexShaderObjectPerVertex,gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(vertexShaderObjectPerVertex);
		if(error.length > 0)
		{
			alert(error);
			uninitialize( );
		}
	}
	
	// fragment shader
	var fragmentShaderSourceCodePerVertex =
		"#version 300 es" +
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
		"}";
	
	fragmentShaderObjectPerVertex=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectPerVertex,fragmentShaderSourceCodePerVertex);
	gl.compileShader(fragmentShaderObjectPerVertex);
	if(gl.getShaderParameter(fragmentShaderObjectPerVertex,gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObjectPerVertex);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}
	
	// shader program
	shaderProgramObjectPerVertex=gl.createProgram();
	gl.attachShader(shaderProgramObjectPerVertex,vertexShaderObjectPerVertex);
	gl.attachShader(shaderProgramObjectPerVertex,fragmentShaderObjectPerVertex);
	
	// pre-link binding of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObjectPerVertex,WebGLMacros.AMC_ATTRIBUTE_VERTEX,"vPosition");
	gl.bindAttribLocation(shaderProgramObjectPerVertex,WebGLMacros.AMC_ATTRIBUTE_NORMAL,"vNormal");
	
	// linking
	gl.linkProgram(shaderProgramObjectPerVertex);
	if (!gl.getProgramParameter(shaderProgramObjectPerVertex, gl.LINK_STATUS))
	{
		var error=gl.getProgramInfoLog(shaderProgramObjectPerVertex); 
		if(error.length > 0) 
		{
			alert(error); 
			uninitialize();
		}
	}
	
	// get MVP uniform location
	mUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_m_matrix");
	vUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_v_matrix");
	pUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_p_matrix");
	laUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_la");
	ldUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_ld");
	lsUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_ls");
	kaUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_ka");
	kdUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_kd");
	ksUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_ks");
	materialShininessUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_material_shininess");
	lightPositionUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_light_position");
	lightingEnabledUniformPerVertex = gl.getUniformLocation(shaderProgramObjectPerVertex, "u_lkeypressed");
	//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
		

	//xx//xx//xx//PerFragmentInitilization//xx//xx//xx//
	//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
	var vertexShaderSourceCodePerFragment =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
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
		"}";
	
	vertexShaderObjectPerFragment=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObjectPerFragment,vertexShaderSourceCodePerFragment);
	gl.compileShader(vertexShaderObjectPerFragment);
	if(gl.getShaderParameter(vertexShaderObjectPerFragment,gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(vertexShaderObjectPerFragment);
		if(error.length > 0)
		{
			alert(error);
			uninitialize( );
		}
	}
	
	// fragment shader
	var fragmentShaderSourceCodePerVertex =
		"#version 300 es" +
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
		"}";
	
	fragmentShaderObjectPerFragment=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObjectPerFragment,fragmentShaderSourceCodePerVertex);
	gl.compileShader(fragmentShaderObjectPerFragment);
	if(gl.getShaderParameter(fragmentShaderObjectPerFragment,gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObjectPerFragment);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}
	
	// shader program
	shaderProgramObjectPerFragment=gl.createProgram();
	gl.attachShader(shaderProgramObjectPerFragment,vertexShaderObjectPerFragment);
	gl.attachShader(shaderProgramObjectPerFragment,fragmentShaderObjectPerFragment);
	
	// pre-link binding of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObjectPerFragment,WebGLMacros.AMC_ATTRIBUTE_VERTEX,"vPosition");
	gl.bindAttribLocation(shaderProgramObjectPerFragment,WebGLMacros.AMC_ATTRIBUTE_NORMAL,"vNormal");
	
	// linking
	gl.linkProgram(shaderProgramObjectPerFragment);
	if (!gl.getProgramParameter(shaderProgramObjectPerFragment, gl.LINK_STATUS))
	{
		var error=gl.getProgramInfoLog(shaderProgramObjectPerFragment); 
		if(error.length > 0) 
		{
			alert(error); 
			uninitialize();
		}
	}
	
	// get MVP uniform location
	mUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_m_matrix");
	vUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_v_matrix");
	pUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_p_matrix");
	laUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_la");
	ldUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_ld");
	lsUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_ls");
	kaUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_ka");
	kdUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_kd");
	ksUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_ks");
	materialShininessUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_material_shininess");
	lightPositionUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_light_position");
	lightingEnabledUniformPerFragment = gl.getUniformLocation(shaderProgramObjectPerFragment, "u_lkeypressed");
	//xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx////xx//xx//xx//
		

	sphere = new Mesh();
	makeSphere(sphere,2.0,30,30);
	
	
	// set clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	// Depth test will always be enabled
	gl.enable(gl.DEPTH_TEST);
	
	// depth test to do
	gl.depthFunc(gl.LEQUAL);
	
	// We will always cull back faces for better performance
	//gl.enable(gl.CULL_FACE);
	
	// initialize projection matrix
	perspectiveProjectionMatrix=mat4.create();
}

function resize()
{
	// code
	if(bFullscreen==true)
	{
		canvas.width=window.innerWidth;
		canvas.height=window.innerHeight;
	}
	else
	{
		canvas.width=canvas_original_width;
		canvas.height=canvas_original_height;
	}
	
	// set the viewport to match
	gl.viewport(0, 0, canvas.width, canvas.height);
	
	mat4.perspective(perspectiveProjectionMatrix, 
						45.0, 
						parseFloat(canvas.width)/ parseFloat(canvas.height), 
						0.1, 
						100.0);
}

function draw()
{
	// code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	if(gbToggle == true)
	{
		gl.useProgram(shaderProgramObjectPerVertex);
		
		var modelMatrixPerVertex=mat4.create();
		var viewMatrixPerVertex=mat4.create();
		var ProjectionMatrixPerVertex = mat4.create();

		if(gbLighting ==  true)
		{
			gl.uniform1i(lightingEnabledUniformPerVertex, 1);
			gl.uniform3fv(laUniformPerVertex, lightAmbient);
			gl.uniform3fv(ldUniformPerVertex, lightDiffuse);
			gl.uniform3fv(lsUniformPerVertex, lightSpecular);
			gl.uniform3fv(kaUniformPerVertex, materialAmbient);
			gl.uniform3fv(kdUniformPerVertex, materialDiffuse);
			gl.uniform3fv(ksUniformPerVertex, materialSpecular);
			gl.uniform1f(materialShininessUniformPerVertex, materialShininess);
			gl.uniform4fv(lightPositionUniformPerVertex, lightPosition);
		}
		else
		{
			gl.uniform1i(lightingEnabledUniformPerVertex, 0);
		}
		
		mat4.translate(modelMatrixPerVertex, modelMatrixPerVertex, [0.0 ,0.0 ,-7.0]);
		
		mat4.multiply(ProjectionMatrixPerVertex, perspectiveProjectionMatrix, ProjectionMatrixPerVertex); 
		
		gl.uniformMatrix4fv(mUniformPerVertex, false, modelMatrixPerVertex);
		gl.uniformMatrix4fv(vUniformPerVertex, false, viewMatrixPerVertex);
		gl.uniformMatrix4fv(pUniformPerVertex, false, ProjectionMatrixPerVertex);
			
		sphere.draw();
		
		gl.useProgram(null);
	}
	else if(gbToggle == false)
	{
		gl.useProgram(shaderProgramObjectPerFragment);
		
		var modelMatrixPerFragment=mat4.create();
		var viewMatrixPerFragment=mat4.create();
		var ProjectionMatrixPerFragment = mat4.create();

		if(gbLighting ==  true)
		{
			gl.uniform1i(lightingEnabledUniformPerFragment, 1);
			gl.uniform3fv(laUniformPerFragment, lightAmbient);
			gl.uniform3fv(ldUniformPerFragment, lightDiffuse);
			gl.uniform3fv(lsUniformPerFragment, lightSpecular);
			gl.uniform3fv(kaUniformPerFragment, materialAmbient);
			gl.uniform3fv(kdUniformPerFragment, materialDiffuse);
			gl.uniform3fv(ksUniformPerFragment, materialSpecular);
			gl.uniform1f(materialShininessUniformPerFragment, materialShininess);
			gl.uniform4fv(lightPositionUniformPerFragment, lightPosition);
		}
		else
		{
			gl.uniform1i(lightingEnabledUniformPerFragment, 0);
		}
		
		mat4.translate(modelMatrixPerFragment, modelMatrixPerFragment, [0.0 ,0.0 ,-7.0]);
		
		mat4.multiply(ProjectionMatrixPerFragment, perspectiveProjectionMatrix, ProjectionMatrixPerFragment); 
		
		gl.uniformMatrix4fv(mUniformPerFragment, false, modelMatrixPerFragment);
		gl.uniformMatrix4fv(vUniformPerFragment, false, viewMatrixPerFragment);
		gl.uniformMatrix4fv(pUniformPerFragment, false, ProjectionMatrixPerFragment);
			
		sphere.draw();
		
		gl.useProgram(null);
	}
	
	// animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	// code
	if(sphere)
	{
		sphere.deallocate();
		sphere=null;
	}
	
	if(shaderProgramObjectPerVertex)
	{
		if(vertexShaderObjectPerVertex)
		{
			gl.detachShader(shaderProgramObjectPerVertex,fragmentShaderObjectPerVertex);
			gl.deleteShader(fragmentShaderObjectPerVertex);
			fragmentShaderObjectPerVertex=null;
		}
		
		if(vertexShaderObjectPerVertex)
		{
			gl.detachShader(shaderProgramObjectPerVertex,vertexShaderObjectPerVertex);
			gl.deleteShader(vertexShaderObjectPerVertex);
			vertexShaderObjectPerVertex=null;
		}
		
		gl.deleteProgram(shaderProgramObjectPerVertex);
		shaderProgramObjectPerVertex=null;
	}

	if(shaderProgramObjectPerFragment)
	{
		if(fragmentShaderObjectPerFragment)
		{
			gl.detachShader(shaderProgramObjectPerFragment,fragmentShaderObjectPerFragment);
			gl.deleteShader(fragmentShaderObjectPerFragment);
			fragmentShaderObjectPerFragment=null;
		}
		
		if(vertexShaderObjectPerFragment)
		{
			gl.detachShader(shaderProgramObjectPerFragment,vertexShaderObjectPerFragment);
			gl.deleteShader(vertexShaderObjectPerFragment);
			vertexShaderObjectPerFragment=null;
		}
		
		gl.deleteProgram(shaderProgramObjectPerFragment);
		shaderProgramObjectPerFragment=null;
	}
}

function keyDown(event)
{
	// code
	switch(event.keyCode)
	{
		case 27: // Escape
			// uninitialize
			uninitialize();
			// close our application's tab
			window.close(); // may not work in Firefox but works in Safari and chrome
			break;
		case 70: // for 'F' or ' f '
			toggleFullScreen();
			break;
		case 76: // for 'L' or 'l'
			if(gbLighting == false)
			{
				gbLighting = true;
			}
			else
			{
				gbLighting = false;
			}
			break;
		case 84: // for 'T' or 't'
			if(gbToggle == false)
			{
				gbToggle = true;
			}
			else if(gbToggle == true)
			{
				gbToggle = false;
			}
			break;

	}
}

function mouseDown()
{
	// code
}