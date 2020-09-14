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

var mvUniform;
var pUniform;
var ldUniform = 0;
var kdUniform = 0;
var lightPositionUniform = 0;
var lightingEnabledUniform = 0;

var gbLighting = false;

var sphere;

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
		"in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_mv_matrix;" +
		"uniform mat4 u_p_matrix;" +
		"uniform mediump int u_lkeypressed;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_kd;" +
		"uniform vec4 u_light_position;" +
		"out vec3 diffuse_color;" +
		"void main(void)" +
		"{" +
		"   if(u_lkeypressed == 1)" +
		"   {" +
		"       vec4 eye_coordinates = u_mv_matrix * vPosition;" +
		"		mat3 normal_matrix = mat3(u_mv_matrix);" +
		"       vec3 tNorm = normalize(normal_matrix * vNormal);" +
		"       vec3 s = normalize(vec3(u_light_position - eye_coordinates));" +
		"       diffuse_color = u_ld * u_kd * max(dot(s, tNorm), 0.0);" +
		"   }" +
		"   gl_Position = u_p_matrix * u_mv_matrix * vPosition;" +
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
		"in vec3 diffuse_color;" +
		"out vec4 FragColor;" +
		"uniform int u_lkeypressed;" +
		"void main(void)" +
		"{" +
		"   if(u_lkeypressed == 1)" +
		"   {" +
		"       FragColor = vec4(diffuse_color, 1.0);" +
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
	mvUniform = gl.getUniformLocation(shaderProgramObject,"u_mv_matrix");
	pUniform = gl.getUniformLocation(shaderProgramObject,"u_p_matrix");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
	lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");
	
	
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
	
	gl.useProgram(shaderProgramObject);

	if(gbLighting)
	{
		gl.uniform1i(lightingEnabledUniform, 1);
		gl.uniform3f(ldUniform, 1.0, 1.0, 1.0);
		gl.uniform3f(kdUniform, 0.5, 0.5, 0.5);

		var lightPosition = [0.0, 0.0, 2.0, 1.0];
		gl.uniform4fv(lightPositionUniform, lightPosition);
	}
	else
	{
		gl.uniform1i(lightingEnabledUniform, 0);
	}
	
	var modelViewMatrix=mat4.create();
	var projectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0 ,0.0 ,-6.0]);
	
	mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, projectionMatrix); 
	
	gl.uniformMatrix4fv(mvUniform, false, modelViewMatrix);
	gl.uniformMatrix4fv(pUniform, false, projectionMatrix);
	
	sphere.draw();
	
	gl.useProgram(null);
	
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
	}
}

function mouseDown()
{
	// code
}