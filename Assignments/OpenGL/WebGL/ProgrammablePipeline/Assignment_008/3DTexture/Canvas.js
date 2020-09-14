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

var uniform_texture0_sampler;

var pyramid_texture = 0;
var cube_texture=0;
var anglePyramid=0.0;
var angleCube=0.0;

var vao_pyramid;
var vao_cube;
var vbo_position;
var vbo_texture; 
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
		"in vec4 vPosition;"+
		"in vec2 vTexture0_Coord;"+
		"out vec2 out_texture0_coord;"+
		"uniform mat4 u_mvp_matrix;"+
		"void main(void)"+
		"{"+
		"	gl_Position = u_mvp_matrix * vPosition;"+
		"	out_texture0_coord = vTexture0_Coord;"+
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
		"in vec2 out_texture0_coord;"+
		"uniform highp sampler2D u_texture0_sampler;"+ 
		"out vec4 FragColor;"+
		"void main(void)"+
		"{"+
		"	FragColor = texture(u_texture0_sampler, out_texture0_coord);"+
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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_TEXTURE0,"vTexture0_Coord");
	
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
	
	// Load pyramid Textures
	pyramid_texture = gl.createTexture();
	pyramid_texture.image = new Image();
	pyramid_texture.image.src = "stone.png";
	pyramid_texture.image.onload = function ()
	{
		gl.bindTexture(gl.TEXTURE_2D, pyramid_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, pyramid_texture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}
	
	// Load cube Textures
	cube_texture = gl.createTexture();
	cube_texture.image = new Image();
	cube_texture.image.src = "Vijay_Kundali.png";
	cube_texture.image.onload = function ()
	{
		gl.bindTexture(gl.TEXTURE_2D, cube_texture);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, cube_texture.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}
	
	// get MVP uniform location
	mvpUniform = gl.getUniformLocation(shaderProgramObject,"u_mvp_matrix");
	// get texture0_sampler uniform location
	uniform_texture0_sampler = gl.getUniformLocation(shaderProgramObject,"u_texture0_sampler");
	
	// *** vertices, colors, shader attribs, vbo, vao initializations ***
	var pyramidVertices = new Float32Array([
												// Front face
												0.0, 1.0, 0.0,		// front top
												-1.0, -1.0, 1.0,	// front left
												1.0, -1.0, 1.0,		// front right

												// Right face
												0.0, 1.0, 0.0,		// right top
												1.0, -1.0, 1.0,		// right left 
												1.0, -1.0, -1.0,	// right right

												// Back face
												0.0, 1.0, 0.0,		// back top
												1.0, -1.0, -1.0,	// back left 
												-1.0, -1.0, -1.0,	// back right

												// Left face
												0.0, 1.0, 0.0,		// left top
												-1.0, -1.0, -1.0,	// left left
												-1.0, -1.0, 1.0		// left right
	]);
	
	var pyramidTexcoords = new Float32Array([
												// Front face
												0.5, 1.0,			// front top
												0.0, 0.0,			// front left
												1.0, 0.0,			// front right
												
												// Right face
												0.5, 1.0,			// right top
												1.0, 0.0,			// right left
												0.0, 0.0,			// right right
												
												// Back face
												0.5, 1.0,			// back top
												1.0, 0.0,			// back left
												0.0, 0.0,			// back right
												
												// Left face
												0.5, 1.0,			// left top
												0.0, 0.0,			// left left
												1.0, 0.0			// left right
	]);
	
	var cubeVertices=new Float32Array([
												// top surface
												1.0, 1.0,-1.0,		// top right of top
												-1.0, 1.0,-1.0,		// top left of top
												-1.0, 1.0, 1.0,		// bottom left of top
												1.0, 1.0, 1.0,		// bottom right of top
												
												// bottom surface
												1.0,-1.0, 1.0,		// top right of bottom
												-1.0,-1.0, 1.0,		// top left of bottom
												-1.0,-1.0,-1.0,		// bottom left of bottom
												1.0,-1.0,-1.0,		// bottom right of bottom
												
												// front surface
												1.0, 1.0, 1.0,		// top right of front
												-1.0, 1.0, 1.0,		// top left of front
												-1.0,-1.0, 1.0,		// bottom left of front
												1.0,-1.0, 1.0,		// bottom right of front
												
												// back surface
												1.0,-1.0,-1.0,		// top right of back
												-1.0,-1.0,-1.0,		// top left of back
												-1.0, 1.0,-1.0,		// bottom left of back
												1.0, 1.0,-1.0,		// bottom right of back
												
												// left surface
												-1.0, 1.0, 1.0,		// top right of left
												-1.0, 1.0,-1.0,		// top left of left
												-1.0,-1.0,-1.0,		// bottom left of left
												-1.0,-1.0, 1.0,		// bottom right of left
												
												// right suface
												1.0, 1.0,-1.0,		// top right of right
												1.0, 1.0, 1.0,		// top left of right
												1.0,-1.0, 1.0,		// bottom left of right
												1.0,-1.0,-1.0		// bottom right of right
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
	
	var cubeTexcoords=new Float32Array([
										0.0,0.0,
										1.0,0.0,
										1.0,1.0,
										0.0,1.0,
										0.0,0.0,
										1.0,0.0,
										1.0,1.0,
										0.0,1.0,
										0.0,0.0,
										1.0,0.0,
										1.0,1.0,
										0.0,1.0,
										0.0,0.0,
										1.0,0.0,
										1.0,1.0,
										0.0,1.0,
										0.0,0.0,
										1.0,0.0,
										1.0,1.0,
										0.0,1.0,
										0.0,0.0,
										1.0,0.0,
										1.0,1.0,
										0.0,1.0,
	]);
	
	// pyramid //
	//vao
	vao_pyramid = gl.createVertexArray();
	gl.bindVertexArray(vao_pyramid);
	
	//Position vbo
	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, pyramidVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z coordinates in our pyramidvertices array.
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//Texture vbo
	vbo_texture = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER,pyramidTexcoords,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0,
							2, //2 is for S and T coordinates in our pyramidTexcoords array.
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	gl.bindVertexArray(null);
	
	// Cube //
	//vao
	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);
	
	//Position vbo
	vbo_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
	gl.bufferData(gl.ARRAY_BUFFER, cubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z coordinates in our pyramidvertices array.
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	
	//Texture vbo
	vbo_texture = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texture);
	gl.bufferData(gl.ARRAY_BUFFER,cubeTexcoords,gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0,
							2, //2 is for S and T coordinates in our pyramidTexcoords array.
							gl.FLOAT, false, 0, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0);
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
	
	// bind with texture
	gl.bindTexture(gl.TEXTURE_2D, pyramid_texture);
	gl.uniform1i(uniform_texture0_sampler, 0);
	
	// draw
	gl.bindVertexArray(vao_pyramid);
	gl.drawArrays(gl.TRIANGLES,0,12); // 3 (each with its x,y,z ) vertices for 4 faces of pyramid
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
	
	// bind with texture
	gl.bindTexture(gl.TEXTURE_2D, cube_texture);
	gl.uniform1i(uniform_texture0_sampler, 0);
	
	// draw
	gl.bindVertexArray(vao_cube);
	
	// draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements 0
	// actually 2 triangles make 1 cube, so there should be 6 vertices,
	// but as 2 tringles while making square meet each other at diagonal,
	// 2 of 6 vertices are common to both triangles, and hence 6-2=4
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
	if(pyramid_texture)
	{
		gl.deleteTexture(pyramid_texture);
		pyramid_texture=0;
	}
	
	if(cube_texture)
	{
		gl.deleteTexture(cube_texture);
		cube_texture=0;
	}
	
	if(vao_pyramid)
	{
		gl.deleteVertexArray(vao_pyramid);
		vao_pyramid=null;
	}
	
	if(vao_cube)
	{
		gl.deleteVertexArray(vao_cube);
		vao_cube=null;
	}
	
	if(vbo_texture)
	{
		gl.deleteBuffer(vbo_texture);
		vbo_texture=null;
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

function degToRad(degrees)
{
	// code
	return(degrees * Math.PI / 180);
}