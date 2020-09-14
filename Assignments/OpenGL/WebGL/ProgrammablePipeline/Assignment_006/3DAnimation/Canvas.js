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
var vbo_color_pyramid;
var vao_cube;
var vbo_position_cube;
var vbo_color_cube;
var mvpUniform;

var anglePyramid = 0.0;
var angleCube = 0.0;
var speed = 1.0;

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
		"#version 300 es"+
		"\n"+
		"in vec4 vPosition;" +
		"in vec4 vColor;" +
		"uniform mat4 u_mvp_matrix;" +
		"out vec4 out_color;" +
		"void main(void)" +
		"{" +
		"   gl_Position = u_mvp_matrix * vPosition;" +
		"	out_color = vColor;" +
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
		"#version 300 es"+
		"\n"+
		"precision highp float;" +
		"in vec4 out_color;" +
		"out vec4 FragColor;" +
		"void main(void)" +
		"{" +
		"	FragColor = out_color;" +
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
	gl.bindAttribLocation(shaderProgramObject, WebGLMacros.AMC_ATTRIBUTE_COLOR, "vColor");
	
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
	mvpUniform = gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
	
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
	
	var pyramidColor = new Float32Array([
												// Front face
												1.0, 0.0, 0.0,
												0.0, 1.0, 0.0,
												0.0, 0.0, 1.0,

												// Right face
												1.0, 0.0, 0.0,
												0.0, 0.0, 1.0,
												0.0, 1.0, 0.0,

												// Back face
												1.0, 0.0, 0.0,
												0.0, 1.0, 0.0,
												0.0, 0.0, 1.0,

												// Left face
												1.0, 0.0, 0.0,
												0.0, 0.0, 1.0,
												0.0, 1.0, 0.0
	]);
	
	var cubeVertices=new Float32Array([
												// Top face
												1.0, 1.0, -1.0,
												-1.0, 1.0, -1.0,
												-1.0, 1.0, 1.0,
												1.0, 1.0, 1.0,

												// Bottom face
												1.0, -1.0, -1.0,
												-1.0, -1.0, -1.0,
												-1.0, -1.0, 1.0,
												1.0, -1.0, 1.0,

												// Front face
												1.0, 1.0, 1.0,
												-1.0, 1.0, 1.0,
												-1.0, -1.0, 1.0,
												1.0, -1.0, 1.0,

												// Back face
												1.0, 1.0, -1.0,
												-1.0, 1.0, -1.0,
												-1.0, -1.0, -1.0,
												1.0, -1.0, -1.0,

												// Right face
												1.0, 1.0, -1.0,
												1.0, 1.0, 1.0,
												1.0, -1.0, 1.0,
												1.0, -1.0, -1.0,

												// Left face
												-1.0, 1.0, 1.0,
												-1.0, 1.0, -1.0,
												-1.0, -1.0, -1.0,
												-1.0, -1.0, 1.0
	]);

	var cubeColor=new Float32Array([
												// Top face
												1.0, 0.0, 0.0,
												1.0, 0.0, 0.0,
												1.0, 0.0, 0.0,
												1.0, 0.0, 0.0,

												// Bottom face
												0.0, 1.0, 0.0,
												0.0, 1.0, 0.0,
												0.0, 1.0, 0.0,
												0.0, 1.0, 0.0,

												// Front face
												0.0, 0.0, 1.0,
												0.0, 0.0, 1.0,
												0.0, 0.0, 1.0,
												0.0, 0.0, 1.0,

												// Back face
												0.0, 1.0, 1.0,
												0.0, 1.0, 1.0,
												0.0, 1.0, 1.0,
												0.0, 1.0, 1.0,

												// right face
												1.0, 0.0, 1.0,
												1.0, 0.0, 1.0,
												1.0, 0.0, 1.0,
												1.0, 0.0, 1.0,

												// left face
												1.0, 1.0, 0.0,
												1.0, 1.0, 0.0,
												1.0, 1.0, 0.0,
												1.0, 1.0, 0.0
	]);	
	
	// If above -1.0 Or +1.0 Values Make Cube Much Larger Than Pyramid,
	// then follow the code in following loop which will convertt all Is And -Is to -0.75 or +0.75
	for(var i=0; i<72; i++)
	{
		if(cubeVertices[i]<0.0)
			cubeVertices[i] = cubeVertices[i]+0.25;
		else if(cubeVertices[i]>0.0)
			cubeVertices[i] = cubeVertices[i]-0.25;
		else
			cubeVertices[i] = cubeVertices[i];
	}
	
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
	
	//Color vbo
	vbo_color_pyramid = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_pyramid);
	gl.bufferData(gl.ARRAY_BUFFER,pyramidColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);
	
	// Cube //
	//vao
	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
	
	//Position vbo
	vbo_position_cube = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX,
							3,
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//Color vbo
	vbo_color_cube = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_color_cube);
	gl.bufferData(gl.ARRAY_BUFFER,cubeColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);
	
	// set clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);
	
	// Depth test will always be enabled
	gl.enable(gl.DEPTH_TEST);
	
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
	
	var modelViewMatrix=mat4.create();
	var modelViewProjectionMatrix = mat4.create();
	
	// pyramid //
	mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -5.0]);
	mat4.rotateY(modelViewMatrix, modelViewMatrix, degToRad(anglePyramid));
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
	
	// draw
	gl.bindVertexArray(vao_pyramid);
	gl.drawArrays(gl.TRIANGLES,0,12);
	gl.bindVertexArray(null);
	
	// cube //
	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -5.0]);
	
	mat4.rotateX(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
	mat4.rotateY(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
	mat4.rotateZ(modelViewMatrix ,modelViewMatrix, degToRad(angleCube));
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
	
	
	// draw
	gl.bindVertexArray(vao_cube);

	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.drawArrays(gl.TRIANGLE_FAN,4,4);
	gl.drawArrays(gl.TRIANGLE_FAN,8,4);
	gl.drawArrays(gl.TRIANGLE_FAN,12,4);
	gl.drawArrays(gl.TRIANGLE_FAN,16,4);
	gl.drawArrays(gl.TRIANGLE_FAN,20,4);
	
	gl.bindVertexArray(null); 
	
	gl.useProgram(null);
	
	anglePyramid=anglePyramid+1.0;
	
	if(anglePyramid>=360.0)
		anglePyramid=anglePyramid-360.0;
	angleCube=angleCube+1.0;
	
	if(angleCube>=360.0)
		angleCube=angleCube-360.0;
	
	// animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	// code	
	if(vbo_color_pyramid)
	{
		gl.deleteBuffer(vbo_color_pyramid);
		vbo_color_pyramid=null;
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
	
	if(vbo_color_cube)
	{
		gl.deleteBuffer(vbo_color_cube);
		vbo_color_cube=null;
	}
	
	if(vbo_position_cube)
	{
		gl.deleteBuffer(vbo_position_cube);
		vbo_position_cube=null;
	}

	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube=null;
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