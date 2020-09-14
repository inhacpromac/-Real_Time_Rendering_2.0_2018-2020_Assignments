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

var vao_graph;
var vbo_graph_position;
var vbo_graph_color;
var vao_triangle;
var vbo_position_triangle;
var vbo_color_triangle;
var vao_square;
var vbo_position_square;
var vbo_color_square;
var vao_circle;
var vbo_circle_position;
var vbo_circle_color;
var mvpUniform;

var numberOfCircleLines = 6000;
var numberOfCircleVertices = numberOfCircleLines * 2 * 3;
var numberOfTriangleVertices = 3 * 3;
var numberOfSquareVertices = 4 * 3;
var numberOfGraphVertices = 1024;
var giNumberOfGraphLines = 0;
var giTotalNumberOfLines = 0;

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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_COLOR,"vColor");
	
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
	var graphVertices = new Float32Array(1024);	
	var graphColor = new Float32Array(1024);	

	var iArrayIndex = 0;
	var fDifference = 1.0 / 20.0;
	for (var fIterator = -1.0; fIterator <= 1.05; fIterator += fDifference)
	{
		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = -1.0;
		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = 0.0;

		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = 1.0;
		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = 0.0;

		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = -1.0;
		graphVertices[iArrayIndex++] = 0.0;

		graphColor[iArrayIndex] = 0;
		graphColor[iArrayIndex + 1] = 0;
		graphColor[iArrayIndex + 2] = 1;

		graphVertices[iArrayIndex++] = fIterator;
		graphVertices[iArrayIndex++] = 1.0;
		graphVertices[iArrayIndex++] = 0.0;
	}

	graphColor[iArrayIndex] = 1;
	graphColor[iArrayIndex + 1] = 0;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = -1.0;
	graphVertices[iArrayIndex++] = 0.0;
	graphVertices[iArrayIndex++] = 0.0;

	graphColor[iArrayIndex] = 1;
	graphColor[iArrayIndex + 1] = 0;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = 1.0;
	graphVertices[iArrayIndex++] = 0.0;
	graphVertices[iArrayIndex++] = 0.0;

	graphColor[iArrayIndex] = 0;
	graphColor[iArrayIndex + 1] = 1;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = 0.0;
	graphVertices[iArrayIndex++] = -1.0;
	graphVertices[iArrayIndex++] = 0.0;

	graphColor[iArrayIndex] = 0;
	graphColor[iArrayIndex + 1] = 1;
	graphColor[iArrayIndex + 2] = 0;

	graphVertices[iArrayIndex++] = 0.0;
	graphVertices[iArrayIndex++] = 1.0;
	graphVertices[iArrayIndex++] = 0.0;

	giNumberOfGraphLines = iArrayIndex / 2;
	
	//console.log(graphVertices);
	//console.log(graphColor);

	// Triangle
	var triangleVertices = new Float32Array([
		0.0, 1.0, 0.0,
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0
	]);
	var triangleColor = new Float32Array([
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0
	]);

	// Square
	var squareVertices = new Float32Array([
		-1.0, 1.0, 0.0,
		-1.0, -1.0, 0.0,
		1.0, -1.0, 0.0,
		1.0, 1.0, 0.0
	]);

	var squareColor = new Float32Array([
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0
	]);
	
	gl.lineWidth(2.0);
	
	// Circle
	var circleVertices =  new Float32Array(numberOfCircleVertices);
	var circleColor =  new Float32Array(numberOfCircleVertices);

	iArrayIndex = 0;
	for (var iIterator = 0; iIterator < numberOfCircleLines; iIterator++)
	{
		circleColor[iArrayIndex] = 1.0;
		circleColor[iArrayIndex + 1] = 1.0;
		circleColor[iArrayIndex + 2] = 0.0;
		circleVertices[iArrayIndex++] = Math.cos((5.0 * 3.14 * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = Math.sin((5.0 * 3.14 * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = 0.0;

		iIterator++;

		circleColor[iArrayIndex] = 1.0;
		circleColor[iArrayIndex + 1] = 1.0;
		circleColor[iArrayIndex + 2] = 0.0;
		circleVertices[iArrayIndex++] = Math.cos((5.0 * 3.14 * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = Math.sin((5.0 * 3.14 * iIterator) / numberOfCircleLines);
		circleVertices[iArrayIndex++] = 0.0;
	}
	
	//console.log(circleVertices);
	//console.log(circleColor);

	//Create vao_graph
	vao_graph = gl.createVertexArray();
	gl.bindVertexArray(vao_graph);
	
	vbo_graph_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_graph_position);
	gl.bufferData(gl.ARRAY_BUFFER,graphVertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_graph_color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_graph_color);
	gl.bufferData(gl.ARRAY_BUFFER,graphColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);

	//Create vao_triangle
	vao_triangle = gl.createVertexArray();
	gl.bindVertexArray(vao_triangle);
	
	vbo_position_triangle = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position_triangle);
	gl.bufferData(gl.ARRAY_BUFFER,triangleVertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_color_triangle = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_color_triangle);
	gl.bufferData(gl.ARRAY_BUFFER,triangleColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);

	//Create vao_square
	vao_square = gl.createVertexArray();
	gl.bindVertexArray(vao_square);
	
	vbo_position_square = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position_square);
	gl.bufferData(gl.ARRAY_BUFFER,squareVertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_color_square = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_color_square);
	gl.bufferData(gl.ARRAY_BUFFER,squareColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);
	
	//Create vao_circle
	vao_circle = gl.createVertexArray();
	gl.bindVertexArray(vao_circle);
	
	vbo_circle_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_circle_position);
	gl.bufferData(gl.ARRAY_BUFFER,circleVertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_circle_color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_circle_color);
	gl.bufferData(gl.ARRAY_BUFFER,circleColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	gl.bindVertexArray(null);
	
	// set clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	gl.enable(gl.DEPTH_TEST);

	gl.depthFunc(gl.LEQUAL);
	
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
	
	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0 ,0.0 ,-3.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
	
	gl.bindVertexArray(vao_graph);
	
	gl.drawArrays(gl.LINES, 0, giNumberOfGraphLines);
	
	gl.bindVertexArray(null);

	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [-0.5, 0.5, -3.0]);
	mat4.scale(modelViewMatrix, modelViewMatrix, [0.5, 0.5, 0.0]);
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao_triangle);
	
	gl.drawArrays(gl.TRIANGLES, 0, 3);
	
	gl.bindVertexArray(null);

	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.5, 0.5, -3.0]);
	mat4.scale(modelViewMatrix, modelViewMatrix, [0.5, 0.5, 0.0]);
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao_square);
	
	gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	
	gl.bindVertexArray(null);

	mat4.identity(modelViewMatrix);
	mat4.identity(modelViewProjectionMatrix);

	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, -0.5, -3.0]);
	mat4.scale(modelViewMatrix, modelViewMatrix, [0.5, 0.5, 0.5]);
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao_circle);
	
	gl.drawArrays(gl.LINES, 0, numberOfCircleLines);
	
	gl.bindVertexArray(null);
	
	gl.useProgram(null);
	
	// animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	// code
	if(vbo_circle_color)
	{
		gl.deleteVertexArray(vbo_circle_color);
		vbo_circle_color=null;
	}
	
	if(vbo_circle_position)
	{
		gl.deleteBuffer(vbo_circle_position);
		vbo_circle_position=null;
	}

	if(vao_circle)
	{
		gl.deleteBuffer(vao_circle);
		vao_circle=null;
	}

	if(vbo_color_square)
	{
		gl.deleteVertexArray(vbo_color_square);
		vbo_color_square=null;
	}
	
	if(vbo_position_square)
	{
		gl.deleteBuffer(vbo_position_square);
		vbo_position_square=null;
	}

	if(vao_square)
	{
		gl.deleteBuffer(vao_square);
		vao_square=null;
	}

	if(vbo_color_triangle)
	{
		gl.deleteVertexArray(vbo_color_triangle);
		vbo_color_triangle=null;
	}
	
	if(vbo_position_triangle)
	{
		gl.deleteBuffer(vbo_position_triangle);
		vbo_position_triangle=null;
	}

	if(vao_triangle)
	{
		gl.deleteBuffer(vao_triangle);
		vao_triangle=null;
	}

	if(vbo_graph_color)
	{
		gl.deleteVertexArray(vbo_graph_color);
		vbo_graph_color=null;
	}
	
	if(vbo_graph_position)
	{
		gl.deleteBuffer(vbo_graph_position);
		vbo_graph_position=null;
	}

	if(vao_graph)
	{
		gl.deleteBuffer(vao_graph);
		vao_graph=null;
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