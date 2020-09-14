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

var vertexShaderObject;
var fragmentShaderObject;
var shaderProgramObject;

var vao_pyramid;
var vbo_position_pyramid;
var vbo_normal_pyramid;
var mUniform;
var vUniform;
var pUniform;
var laZeroUniform = 0;
var ldZeroUniform = 0;
var lsZeroUniform = 0;
var laOneUniform = 0;
var ldOneUniform = 0;
var lsOneUniform = 0;
var kaUniform = 0;
var kdUniform = 0;
var ksUniform = 0;
var materialShininessUniform = 0;
var lightZeroPositionUniform = 0;
var lightOnePositionUniform = 0;
var lightingEnabledUniform = 0;
var anglePyramid = 0.0;
var speed = 1.0;

var gbLighting = false;
var gbAnimation = false;

var lightZeroAmbient = [ 0.0,0.0,0.0 ];
var lightZeroDiffuse = [ 1.0,0.0,0.0 ];
var lightZeroSpecular = [ 1.0,1.0,1.0 ];
var lightZeroPosition = [ 2.0,1.0,1.0,0.0 ];

var lightOneAmbient = [ 0.0,0.0,0.0 ];
var lightOneDiffuse = [ 0.0,0.0,1.0 ];
var lightOneSpecular = [ 0.0,0.0,1.0 ];
var lightOnePosition = [ -2.0,1.0,1.0,0.0 ];

var materialAmbient = [ 0.0,0.0,0.0 ];
var materialDiffuse = [ 1.0,1.0,1.0 ];
var materialSpecular = [ 1.0,1.0,1.0 ];
var materialShininess = 128.0; //50.0f

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
	
	// vertex shader
	var vertexShaderSourceCode =
		"#version 300 es" +
		"\n" +
		"precision highp float;" +
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_m_matrix;" +
		"uniform mat4 u_v_matrix;" +
		"uniform mat4 u_p_matrix;" +
		"uniform mediump int u_lkeypressed;" +
		"uniform vec3 u_la_zero;" +
		"uniform vec3 u_ld_zero;" +
		"uniform vec3 u_ls_zero;" +
		"uniform vec3 u_la_one;" +
		"uniform vec3 u_ld_one;" +
		"uniform vec3 u_ls_one;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform vec4 u_light_position_zero;" +
		"uniform vec4 u_light_position_one;" +
		"uniform float u_material_shininess;" +
		"out vec3 phong_ads_light;" +
		"void main(void)" +
		"{" +
		"   if(u_lkeypressed == 1)" +
		"   {" +
		"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
		"       vec3 tNorm = normalize(mat3(u_v_matrix * u_m_matrix) * vNormal);" +
		"       vec3 light_direction_zero = normalize(vec3(u_light_position_zero - eye_coordinates));" +
		"       vec3 light_direction_one = normalize(vec3(u_light_position_one - eye_coordinates));" +
		"       float tn_dot_ld_zero = max(dot(light_direction_zero, tNorm), 0.0);" +
		"       float tn_dot_ld_one = max(dot(light_direction_one, tNorm), 0.0);" +
		"       vec3 reflection_vector_zero = reflect(-light_direction_zero, tNorm);" +
		"       vec3 reflection_vector_one = reflect(-light_direction_one, tNorm);" +
		"       vec3 viewer_vector = normalize(-eye_coordinates.xyz);" +
		"       vec3 ambient_zero = u_la_zero * u_ka;" +
		"       vec3 ambient_one = u_la_one * u_ka;" +
		"       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" +
		"       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" +
		"       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, viewer_vector), 0.0), u_material_shininess);" +
		"       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, viewer_vector), 0.0), u_material_shininess);" +
		"       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one;" +
		"   }" +
		"   else" +
		"   {" +
		"       phong_ads_light = vec3(1.0, 1.0, 1.0);" +
		"   }" +
		"   gl_Position = u_p_matrix * u_v_matrix* u_m_matrix * vPosition;" +
		"}";
	vertexShaderObject=gl.createShader(gl.VERTEX_SHADER);
	gl.shaderSource(vertexShaderObject,vertexShaderSourceCode);
	gl.compileShader(vertexShaderObject);
	if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(vertexShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize( );
		}
	}
	
	// fragment shader
	var fragmentShaderSourceCode =
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
	fragmentShaderObject=gl.createShader(gl.FRAGMENT_SHADER);
	gl.shaderSource(fragmentShaderObject,fragmentShaderSourceCode);
	gl.compileShader(fragmentShaderObject);
	if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS)==false)
	{
		var error=gl.getShaderInfoLog(fragmentShaderObject);
		if(error.length > 0)
		{
			alert(error);
			uninitialize();
		}
	}
	
	// shader program
	shaderProgramObject=gl.createProgram();
	gl.attachShader(shaderProgramObject,vertexShaderObject);
	gl.attachShader(shaderProgramObject,fragmentShaderObject);
	
	// pre-link binding of shader program object with vertex shader attributes
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_VERTEX,"vPosition");
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_NORMAL,"vNormal");
	
	// linking
	gl.linkProgram(shaderProgramObject);
	if (!gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS))
	{
		var error=gl.getProgramInfoLog(shaderProgramObject); 
		if(error.length > 0) 
		{
			alert(error); 
			uninitialize();
		}
	}
	
	// get MVP uniform location
	mUniform = gl.getUniformLocation(shaderProgramObject, "u_m_matrix");
	vUniform = gl.getUniformLocation(shaderProgramObject, "u_v_matrix");
	pUniform = gl.getUniformLocation(shaderProgramObject, "u_p_matrix");
	laZeroUniform = gl.getUniformLocation(shaderProgramObject, "u_la_zero");
	ldZeroUniform = gl.getUniformLocation(shaderProgramObject, "u_ld_zero");
	lsZeroUniform = gl.getUniformLocation(shaderProgramObject, "u_ls_zero");
	laOneUniform = gl.getUniformLocation(shaderProgramObject, "u_la_one");
	ldOneUniform = gl.getUniformLocation(shaderProgramObject, "u_ld_one");
	lsOneUniform = gl.getUniformLocation(shaderProgramObject, "u_ls_one");
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");
	lightZeroPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position_zero");
	lightOnePositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position_one");
	lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");
	
	
	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	var pyramidVertices = new Float32Array([
			// Front face
			0.0, 1.0, 0.0,
			-1.0, -1.0, 1.0,
			1.0, -1.0, 1.0,

			// Right face
			0.0, 1.0, 0.0,
			1.0, -1.0, 1.0,
			1.0, -1.0, -1.0,

			// Back face
			0.0, 1.0, 0.0,
			1.0, -1.0, -1.0,
			-1.0, -1.0, -1.0,

			// Left face
			0.0, 1.0, 0.0,
			-1.0, -1.0, -1.0,
			-1.0, -1.0, 1.0
	]);

	var pyramidNormal = new Float32Array([
			// Front face
			0.0, 0.447214, 0.894427,
			0.0, 0.447214, 0.894427,
			0.0, 0.447214, 0.894427,

			// Right face
			0.894427, 0.447214, 0.0,
			0.894427, 0.447214, 0.0,
			0.894427, 0.447214, 0.0,

			// Back face
			0.0, 0.447214, -0.894427,
			0.0, 0.447214, -0.894427,
			0.0, 0.447214, -0.894427,

			// Left face
			-0.894427, 0.447214, 0.0,
			-0.894427, 0.447214, 0.0,
			-0.894427, 0.447214, 0.0
	]);
	
	// pyramid //
	//vao
	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);
	
	//Position vbo
	vbo_position_pyramid = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX,
							3,
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//Normal vbo
	vbo_normal_pyramid = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_normal_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER,pyramidNormal,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_NORMAL,
							3,
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_NORMAL);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);
	
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
	
	gl.useProgram(shaderProgramObject);

	if(gbLighting ==  true)
	{
		gl.uniform1i(lightingEnabledUniform, 1);
		gl.uniform3fv(laZeroUniform, lightZeroAmbient);
		gl.uniform3fv(ldZeroUniform, lightZeroDiffuse);
		gl.uniform3fv(lsZeroUniform, lightZeroSpecular);
		gl.uniform3fv(laOneUniform, lightOneAmbient);
		gl.uniform3fv(ldOneUniform, lightOneDiffuse);
		gl.uniform3fv(lsOneUniform, lightOneSpecular);
		gl.uniform3fv(kaUniform, materialAmbient);
		gl.uniform3fv(kdUniform, materialDiffuse);
		gl.uniform3fv(ksUniform, materialSpecular);
		gl.uniform1f(materialShininessUniform, materialShininess);
		gl.uniform4fv(lightZeroPositionUniform, lightZeroPosition);
		gl.uniform4fv(lightOnePositionUniform, lightOnePosition);
	}
	else
	{
		gl.uniform1i(lightingEnabledUniform, 0);
	}
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	var projectionMatrix = mat4.create();
	
	mat4.translate(modelMatrix, modelMatrix, [0.0 ,0.0 ,-6.0]);
	mat4.rotateY(modelMatrix, modelMatrix, degToRad(anglePyramid));
	
	mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, projectionMatrix); 
	
	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, projectionMatrix);
		
	// draw
	gl.bindVertexArray(vao_pyramid);
	gl.drawArrays(gl.TRIANGLES,0,12);
	gl.bindVertexArray(null);
	
	gl.useProgram(null);

	if(gbAnimation == true)
	{
		anglePyramid=anglePyramid+1.0;
		
		if(anglePyramid>=360.0)
			anglePyramid=anglePyramid-360.0;
	}
	
	
	// animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	// code
	if(vbo_normal_pyramid)
	{
		gl.deleteBuffer(vbo_normal_pyramid);
		vbo_normal_pyramid=null;
	}
	
	if(vbo_position_pyramid)
	{
		gl.deleteBuffer(vbo_position_pyramid);
		vbo_position_pyramid=null;
	}

	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid=null;
	}
	
	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject,fragmentShaderObject);
			gl.deleteShader(fragmentShaderObject);
			fragmentShaderObject=null;
		}
		
		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject,vertexShaderObject);
			gl.deleteShader(vertexShaderObject);
			vertexShaderObject=null;
		}
		
		gl.deleteProgram(shaderProgramObject);
		shaderProgramObject=null;
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
		case 76:
			if(gbLighting == false)
			{
				gbLighting = true;
			}
			else
			{
				gbLighting = false;
			}
		case 65:
			if(gbAnimation == false)
			{
				gbAnimation = true;
			}
			else
			{
				gbAnimation = false;
			}
	}
}

function mouseDown()
{
	// code
}

function degToRad(degrees)
{
	// code
	return(degrees * Math.PI / 180);
}
