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

var vao;
var vbo_position;
var vbo_color;
var mvpUniform;

var transI1 = -10.0;
var transN = 15.0;
var transD = 15.0;
var transI2 = -15.0;
var transA = 15.0;

var bDrawN = false;
var bDrawI2 = false;
var bDrawA = false;
var bDrawD = false;

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
	mvpUniform=gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
	
	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	var Vertices = new Float32Array([
		-5.0, -2.0, 0.0, 
		-5.0, 2.0, 0.0, 
		-3.0, -2.0, 0.0, 
		-3.0, 2.0, 0.0, 
		-3.0, 2.0, 0.0, 
		-1.0, -2.0, 0.0, 
		-1.0, -2.0, 0.0, 
		-1.0, 2.0, 0.0, 
		0.0, 2.0, 0.0, 
		2.0, 2.0, 0.0, 
		0.5, -2.0, 0.0, 
		0.5, 2.0, 0.0, 
		0.0, -2.0, 0.0, 
		2.0, -2.0, 0.0, 
		2.0, -2.0, 0.0, 
		2.0, 2.0, 0.0, 
		3.0, -2.0, 0.0, 
		3.0, 2.0, 0.0, 
		4.0, 2.0, 0.0, 
		6.0, 2.0, 0.0, 
		4.0, 0.0, 0.0, 
		6.0, 0.0, 0.0, 
		4.0, -2.0, 0.0, 
		4.0, 2.0, 0.0, 
		6.0, -2.0, 0.0,
		6.0, 2.0, 0.0
		]);

	var Color = new Float32Array([
		0.0,  1.0,  0.0, 
		0.6,  0.2,  0.0, 
		0.0,  1.0,  0.0, 
		0.6,  0.2,  0.0, 
		0.6,  0.2,  0.0, 
		0.0,  1.0,  0.0, 
		0.0,  1.0,  0.0,  
		0.6,  0.2,  0.0, 
		0.6,  0.2,  0.0, 
		0.6,  0.2,  0.0, 
		0.0,  1.0,  0.0,  
		0.6,  0.2,  0.0, 
		0.0,  1.0,  0.0,  
		0.0,  1.0,  0.0,  
		0.0,  1.0,  0.0,  
		0.6,  0.2,  0.0, 
		0.0,  1.0,  0.0,  
		0.6,  0.2,  0.0, 
		0.6,  0.2,  0.0, 
		0.6,  0.2,  0.0, 
		1.0,  1.0,  1.0,  
		1.0,  1.0,  1.0,  
		0.0,  1.0,  0.0,  
		0.6,  0.2,  0.0, 
		0.0,  1.0,  0.0, 
		0.6,  0.2, 0.0
	]);

	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);
	
	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER,Vertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_color);
	gl.bufferData(gl.ARRAY_BUFFER,Color,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	gl.bindVertexArray(null);

	gl.lineWidth(10);
	
	// set clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);
	
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
	gl.clear(gl.COLOR_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	var modelViewMatrix=mat4.create();
	var modelViewProjectionMatrix = mat4.create();



	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [transI1 ,0.0 ,-15.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	gl.drawArrays(gl.LINES, 0, 2);
	
	gl.bindVertexArray(null);



	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [-1.0 ,transN ,-15.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	gl.drawArrays(gl.LINES, 2, 7);
	
	gl.bindVertexArray(null);



	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [1.0 ,transI2 ,-15.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	gl.drawArrays(gl.LINES, 16, 2);
	
	gl.bindVertexArray(null);



	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [transA ,0.0 ,-15.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	gl.drawArrays(gl.LINES, 18, 8);
	
	gl.bindVertexArray(null);



	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0 ,0.0 ,transD]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	gl.drawArrays(gl.LINES, 8, 8);
	
	gl.bindVertexArray(null);




	
	gl.useProgram(null);

	transI1 += 0.1;
		if (transI1 >= -1.0)
		{
			bDrawN = true;
			transI1 = -1.0;
		}
		if (bDrawN == true)
		{
			transN -= 0.1;
			if (transN < 0.0)
			{
				bDrawI2 = true;
				transN = 0.0;
			}
		}
		if (bDrawI2 == true)
		{
			transI2 += 0.1;
			if (transI2 > 0.0)
			{
				bDrawA = true;
				transI2 = 0.0;
			}
		}
		if (bDrawA == true)
		{
			transA -= 0.1;
			if (transA < 2.0)
			{
				bDrawD = true;
				transA = 2.0;
			}
		}
		if (bDrawD == true)
		{
			transD -= 0.1;
			if (transD < -15.0)
			{
				transD = -15.0;
			}
		}
	
	// animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	// code
	if(vao)
	{
		gl.deleteVertexArray(vao);
		vao=null;
	}
	
	if(vbo_color)
	{
		gl.deleteBuffer(vbo_color);
		vbo_color=null;
	}

	if(vbo_position)
	{
		gl.deleteBuffer(vbo_position);
		vbo_position=null;
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