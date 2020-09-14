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

//vao-shapes
//vbo-color,texture,....
var vao;
var vbo_position;
var vbo_color;

var vbo_point_element;
var pointElement = 0;

var vbo_lines_element;
var lineElement = 0;

var vbo_square_element;
var squareElement = 0;

var vbo_diagonal_square_element;
var diaSquareElement = 0;

var vbo_fan_element;
var fanElement = 0;

var vao_color_square;
var vbo_color_square;
var vbo_color_square_element;
var colorSquareElement = 0;

var mvpUniform;

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
		"in vec4 vPosition ;"+
		"in vec4 vColor;"+
		"uniform mat4 u_mvp_matrix;"+
		"out vec4 out_color;"+
		"void main(void)"+
		"{"+
		"	gl_Position = u_mvp_matrix * vPosition;"+
		"	out_color = vColor;"+
		"	gl_PointSize = 5.0;"+
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
		"precision highp float;"+
		"in vec4 out_color;" +
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
		"	FragColor = out_color;"+
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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_COLOR, "vColor");
	
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
	var vertices = new Float32Array([
		-1.0, 1.0, 0.0,
		-0.5, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.5, 1.0, 0.0,

		-1.0, 0.5, 0.0,
		-0.5, 0.5, 0.0,
		0.0, 0.5, 0.0,
		0.5, 0.5, 0.0,

		-1.0, 0.0, 0.0,
		-0.5, 0.0, 0.0,
		0.0, 0.0, 0.0,
		0.5, 0.0, 0.0,

		-1.0, -0.5, 0.0,
		-0.5, -0.5, 0.0,
		0.0, -0.5, 0.0,
		0.5, -0.5, 0.0
	]);

	var squareColor = new Float32Array([
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 1.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 1.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 1.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 1.0
	]);

	var color = new Float32Array([
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0,
		1.0
		]);

	vao = gl.createVertexArray();
	gl.bindVertexArray(vao);

	//Position of vertices
	vbo = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo);

	gl.bufferData(gl.ARRAY_BUFFER,vertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);
	
	//Points
	var pointIndices =  new Int32Array(16);
	pointElement = 16;
	for (var iItr = 0; iItr < 16; iItr++)
	{
		pointIndices[iItr] = iItr;
	}
	vbo_point_element = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,vbo_point_element);

	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,pointIndices,gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,null);

	//Lines
	var lineIndices =  new Int32Array(30);
	var iIndex = 0;
	lineIndices[iIndex++] = 0;
	lineIndices[iIndex++] = 12;
	lineIndices[iIndex++] = 1;
	lineIndices[iIndex++] = 13;
	lineIndices[iIndex++] = 2;
	lineIndices[iIndex++] = 14;

	lineIndices[iIndex++] = 0;
	lineIndices[iIndex++] = 3;
	lineIndices[iIndex++] = 4;
	lineIndices[iIndex++] = 7;
	lineIndices[iIndex++] = 8;
	lineIndices[iIndex++] = 11;

	lineIndices[iIndex++] = 4;
	lineIndices[iIndex++] = 1;
	lineIndices[iIndex++] = 8;
	lineIndices[iIndex++] = 2;
	lineIndices[iIndex++] = 12;
	lineIndices[iIndex++] = 3;
	lineIndices[iIndex++] = 13;
	lineIndices[iIndex++] = 7;
	lineIndices[iIndex++] = 14;
	lineIndices[iIndex++] = 11;

	lineElement = iIndex;

	vbo_lines_element = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,vbo_lines_element);

	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,lineIndices,gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,null);

	//Squares
	var squareIndices =  new Int32Array(30);
	iIndex = 0;
	squareIndices[iIndex++] = 0;
	squareIndices[iIndex++] = 12;
	squareIndices[iIndex++] = 1;
	squareIndices[iIndex++] = 13;
	squareIndices[iIndex++] = 2;
	squareIndices[iIndex++] = 14;
	squareIndices[iIndex++] = 3;
	squareIndices[iIndex++] = 15;

	squareIndices[iIndex++] = 0;
	squareIndices[iIndex++] = 3;
	squareIndices[iIndex++] = 4;
	squareIndices[iIndex++] = 7;
	squareIndices[iIndex++] = 8;
	squareIndices[iIndex++] = 11;
	squareIndices[iIndex++] = 12;
	squareIndices[iIndex++] = 15;

	squareElement = iIndex;
	vbo_square_element = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,vbo_square_element);

	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,squareIndices,gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,null);

	//Square with Diagonal lines
	var diagonalSquareIndices =  new Int32Array(30);
	iIndex = 0;
	diagonalSquareIndices[iIndex++] = 0;
	diagonalSquareIndices[iIndex++] = 12;
	diagonalSquareIndices[iIndex++] = 1;
	diagonalSquareIndices[iIndex++] = 13;
	diagonalSquareIndices[iIndex++] = 2;
	diagonalSquareIndices[iIndex++] = 14;
	diagonalSquareIndices[iIndex++] = 3;
	diagonalSquareIndices[iIndex++] = 15;

	diagonalSquareIndices[iIndex++] = 0;
	diagonalSquareIndices[iIndex++] = 3;
	diagonalSquareIndices[iIndex++] = 4;
	diagonalSquareIndices[iIndex++] = 7;
	diagonalSquareIndices[iIndex++] = 8;
	diagonalSquareIndices[iIndex++] = 11;
	diagonalSquareIndices[iIndex++] = 12;
	diagonalSquareIndices[iIndex++] = 15;

	diagonalSquareIndices[iIndex++] = 4;
	diagonalSquareIndices[iIndex++] = 1;
	diagonalSquareIndices[iIndex++] = 8;
	diagonalSquareIndices[iIndex++] = 2;
	diagonalSquareIndices[iIndex++] = 12;
	diagonalSquareIndices[iIndex++] = 3;
	diagonalSquareIndices[iIndex++] = 13;
	diagonalSquareIndices[iIndex++] = 7;
	diagonalSquareIndices[iIndex++] = 14;
	diagonalSquareIndices[iIndex++] = 11;

	diaSquareElement = iIndex;
	vbo_diagonal_square_element = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,vbo_diagonal_square_element);

	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,diagonalSquareIndices,gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,null);

	//Fan
	var fanIndices =  new Int32Array(30);
	iIndex = 0;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 12;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 13;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 14;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 15;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 11;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 7;
	fanIndices[iIndex++] = 0;
	fanIndices[iIndex++] = 3;
	fanIndices[iIndex++] = 12;
	fanIndices[iIndex++] = 15;
	fanIndices[iIndex++] = 15;
	fanIndices[iIndex++] = 3;

	fanElement = iIndex;
	vbo_fan_element = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,vbo_fan_element);

	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,fanIndices,gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,null);

	//Color
	vbo_color = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_color);

	gl.bufferData(gl.ARRAY_BUFFER,color,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	gl.bindVertexArray(null);

	//SquaresWithColorVAO
	vao_color_square = gl.createVertexArray();
	gl.bindVertexArray(vao_color_square);
	//Color Squares
	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position);

	gl.bufferData(gl.ARRAY_BUFFER,vertices,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	//Colored Squares
	var coloredVertices = new Float32Array([
		1.0, 1.0, 0.0,
		1.0, 0.5, 0.0,
		1.0, 0.0, 0.0,
		1.0, -0.5, 0.0,

		0.5, 1.0, 0.0,
		0.5, 0.5, 0.0,
		0.5, 0.0, 0.0,
		0.5, -0.5, 0.0,

		0.5, 1.0, 0.0,
		0.5, 0.5, 0.0,
		0.5, 0.0, 0.0,
		0.5, -0.5, 0.0,

		0.0, 1.0, 0.0,
		0.0, 0.5, 0.0,
		0.0, 0.0, 0.0,
		0.0, -0.5, 0.0,

		0.0, 1.0, 0.0,
		0.0, 0.5, 0.0,
		0.0, 0.0, 0.0,
		0.0, -0.5, 0.0,

		-0.5, 1.0, 0.0,
		-0.5, 0.5, 0.0,
		-0.5, 0.0, 0.0,
		-0.5, -0.5, 0.0,
	]);

	var coloredSquareColor = new Float32Array([
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,

		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,

		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,

		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0
	]);

	var colorSquareIndices = new Int32Array(300);
	iIndex = 0;
	colorSquareIndices[iIndex++] = 16;
	colorSquareIndices[iIndex++] = 20;
	colorSquareIndices[iIndex++] = 21;

	colorSquareIndices[iIndex++] = 21;
	colorSquareIndices[iIndex++] = 16;
	colorSquareIndices[iIndex++] = 17;

	colorSquareIndices[iIndex++] = 17;
	colorSquareIndices[iIndex++] = 21;
	colorSquareIndices[iIndex++] = 22;

	colorSquareIndices[iIndex++] = 22;
	colorSquareIndices[iIndex++] = 17;
	colorSquareIndices[iIndex++] = 18;

	colorSquareIndices[iIndex++] = 18;
	colorSquareIndices[iIndex++] = 22;
	colorSquareIndices[iIndex++] = 23;

	colorSquareIndices[iIndex++] = 23;
	colorSquareIndices[iIndex++] = 18;
	colorSquareIndices[iIndex++] = 19;

	colorSquareIndices[iIndex++] = 15;
	colorSquareIndices[iIndex++] = 11;
	colorSquareIndices[iIndex++] = 10;

	colorSquareIndices[iIndex++] = 10;
	colorSquareIndices[iIndex++] = 15;
	colorSquareIndices[iIndex++] = 14;

	colorSquareIndices[iIndex++] = 14;
	colorSquareIndices[iIndex++] = 10;
	colorSquareIndices[iIndex++] = 9;

	colorSquareIndices[iIndex++] = 9;
	colorSquareIndices[iIndex++] = 14;
	colorSquareIndices[iIndex++] = 13;

	colorSquareIndices[iIndex++] = 13;
	colorSquareIndices[iIndex++] = 9;
	colorSquareIndices[iIndex++] = 8;

	colorSquareIndices[iIndex++] = 8;
	colorSquareIndices[iIndex++] = 13;
	colorSquareIndices[iIndex++] = 12;

	colorSquareIndices[iIndex++] = 4;
	colorSquareIndices[iIndex++] = 0;
	colorSquareIndices[iIndex++] = 1;

	colorSquareIndices[iIndex++] = 1;
	colorSquareIndices[iIndex++] = 4;
	colorSquareIndices[iIndex++] = 5;

	colorSquareIndices[iIndex++] = 5;
	colorSquareIndices[iIndex++] = 1;
	colorSquareIndices[iIndex++] = 2;

	colorSquareIndices[iIndex++] = 2;
	colorSquareIndices[iIndex++] = 5;
	colorSquareIndices[iIndex++] = 6;

	colorSquareIndices[iIndex++] = 6;
	colorSquareIndices[iIndex++] = 2;
	colorSquareIndices[iIndex++] = 3;

	colorSquareIndices[iIndex++] = 3;
	colorSquareIndices[iIndex++] = 6;
	colorSquareIndices[iIndex++] = 7;


	colorSquareElement = iIndex;

	//SquaresWithColorVAO
	vao_color_square = gl.createVertexArray();
	gl.bindVertexArray(vao_color_square);

	//Color Squares
	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER,vbo_position);

	gl.bufferData(gl.ARRAY_BUFFER,coloredVertices,gl.STATIC_DRAW);
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

	gl.bufferData(gl.ARRAY_BUFFER,coloredSquareColor,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR, 
		3,
		gl.FLOAT, 
		false, 
		0, 
		0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	gl.bindBuffer(gl.ARRAY_BUFFER,null);

	vbo_color_square_element = gl.createBuffer();
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,vbo_color_square_element);

	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,colorSquareIndices,gl.STATIC_DRAW);
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER,null);

	gl.bindVertexArray(null);

	// set clear color
	gl.clearColor(0.0, 0.0, 0.0, 1.0);

	// Depth test will always be enabled
	gl.enable(gl.DEPTH_TEST);
	
	// depth test to do
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
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	gl.useProgram(shaderProgramObject);
	
	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [-3.0, 1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_point_element);
	gl.drawElements(gl.POINTS, pointElement, gl.UNSIGNED_INT, 0);

	gl.bindVertexArray(null);

	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_lines_element);
	gl.drawElements(gl.LINES, lineElement, gl.UNSIGNED_INT, 0);
	
	gl.bindVertexArray(null);

	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [3.0, 1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_square_element);
	gl.drawElements(gl.LINES, squareElement, gl.UNSIGNED_INT, 0);
	
	gl.bindVertexArray(null);

	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [-3.0, -1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_diagonal_square_element);
	gl.drawElements(gl.LINES, diaSquareElement, gl.UNSIGNED_INT, 0);
	
	gl.bindVertexArray(null);

	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, -1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_fan_element);
	gl.drawElements(gl.LINES, fanElement, gl.UNSIGNED_INT, 0);
	
	gl.bindVertexArray(null);

	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [2.5, -1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao_color_square);
	
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_color_square_element);
	gl.drawElements(gl.TRIANGLES, colorSquareElement, gl.UNSIGNED_INT, 0);
	
	gl.bindVertexArray(null);

	var modelViewMatrix=mat4.create(); //initialize as identity matrix
	var modelViewProjectionMatrix = mat4.create();
	
	mat4.translate(modelViewMatrix, modelViewMatrix, [3.0, -1.0, -8.0]); 
	
	mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix); 
	
	gl.uniformMatrix4fv(mvpUniform, false, modelViewProjectionMatrix);
		
	gl.bindVertexArray(vao);
	
	
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, vbo_square_element);
	gl.drawElements(gl.LINES, squareElement, gl.UNSIGNED_INT, 0);
	
	gl.bindVertexArray(null);
	
	gl.useProgram(null);
	
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
	
	if(vbo_position)
	{
		gl.deleteBuffer(vbo_position);
		vbo_position=null;
	}

	if(vbo_color)
	{
		gl.deleteBuffer(vbo_color);
		vbo_color=null;
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