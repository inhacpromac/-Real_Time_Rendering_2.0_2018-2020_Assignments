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

var vao_interleaved;
var vbo_interleaved;

var lightAmbient=[0.25,0.25,0.25];
var lightDiffuse=[1.0,1.0,1.0];
var lightSpecular=[1.0,1.0,1.0];
var lightPosition=[100.0,100.0,100.0,1.0];

var materialAmbient= [0.0,0.0,0.0];
var materialDiffuse= [1.0,1.0,1.0];
var materialSpecular= [1.0,1.0,1.0];
var materialShininess= 128.0; //50.0

var mUniform;
var vUniform;
var pUniform;
var laUniform = 0;
var ldUniform = 0;
var lsUniform = 0;
var kaUniform = 0;
var kdUniform = 0;
var ksUniform = 0;
var materialShininessUniform = 0;
var lightPositionUniform = 0;
var lightingEnabledUniform = 0;

var angleCube = 0.0;
var speed = 1.0;

var uniform_texture0_sampler;
var texture_marble=0;

var gbLighting = false;
var gbAnimation = false;

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
		"in vec4 vColor;" +
		"in vec3 vNormal;" +
		"in vec2 vTexCoord;" +
		"\n" +
		"uniform mat4 u_m_matrix;" +
		"uniform mat4 u_v_matrix;" +
		"uniform mat4 u_p_matrix;" +
		"uniform mediump int u_lkeypressed;" +
		"uniform vec4 u_light_position;" +
		"\n" +
		"out vec3 tNorm;" +
		"out vec2 out_texcoord;" +
		"out vec3 light_direction;" +
		"out vec3 viewer_vector;" +
		"out vec4 out_color;" +
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
		"	out_color = vColor;" +
		"	out_texcoord = vTexCoord;" +
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
		"in vec3 tNorm;" +
		"in vec3 light_direction;" +
		"in vec3 viewer_vector;" +
		"in vec2 out_texcoord;" +
		"in vec4 out_color;" +
		"\n" +
		"uniform mediump int u_lkeypressed;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_ks;" +
		"uniform float u_material_shininess;" +
		"uniform sampler2D u_sampler;" +
		"\n" +
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
		"	FragColor = texture(u_sampler, out_texcoord) * out_color * vec4(phong_ads_light, 1.0);" +
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
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_NORMAL,"vNormal");
	gl.bindAttribLocation(shaderProgramObject,WebGLMacros.AMC_ATTRIBUTE_TEXTURE0, "vTexCoord");
	
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
	texture_marble = gl.createTexture();
	texture_marble.image = new Image();
	texture_marble.image.src = "marble.png";
	texture_marble.image.onload = function ()
	{
		gl.bindTexture(gl.TEXTURE_2D, texture_marble);
		gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, texture_marble.image);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
		gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
		gl.bindTexture(gl.TEXTURE_2D, null);
	}
	
	// get MVP uniform location
	mUniform = gl.getUniformLocation(shaderProgramObject, "u_m_matrix");
	vUniform = gl.getUniformLocation(shaderProgramObject, "u_v_matrix");
	pUniform = gl.getUniformLocation(shaderProgramObject, "u_p_matrix");
	laUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
	lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");
	uniform_texture0_sampler = gl.getUniformLocation(shaderProgramObject,"u_sampler");
	
	
	//Below have this order for every vertex at (x,y,z)
	//x, y, z, cx, cy, cz, nx, ny, nz, tx, ty
	var interleaved_data = new Float32Array([
		// Top face
		1.0, 1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
		-1.0, 1.0, -1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0,
		-1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0,

		// Bottom face
		1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 1.0,
		-1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 1.0,
		-1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0,
		1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0,

		// Front face
		1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,
		-1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0,
		-1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
		1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,

		// Back face
		1.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 1.0, 0.0,
		-1.0, 1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 1.0, 1.0,
		-1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 1.0,
		1.0, -1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, -1.0, 0.0, 0.0,

		// Right face
		1.0, 1.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0,
		1.0, 1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0,
		1.0, -1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0,
		1.0, -1.0, -1.0, 1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0,

		// Left face
		-1.0, 1.0, 1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0,
		-1.0, 1.0, -1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 0.0,
		-1.0, -1.0, -1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 1.0, 1.0,
		-1.0, -1.0, 1.0, 1.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 1.0
	]);
	
	
	vao_interleaved = gl.createVertexArray();
	gl.bindVertexArray(vao_interleaved);
	
	vbo_interleaved = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_interleaved);
	
	gl.bufferData(gl.ARRAY_BUFFER, interleaved_data, gl.STATIC_DRAW);
	
	//Position vbo
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_VERTEX,
							3, //3 is for X,Y,Z coordinates
							gl.FLOAT, false, 11 * 4, 0);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_VERTEX);
	//Color vbo
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR,
							3,
							gl.FLOAT, false, 11 * 4, 3 * 4);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
	//Normal vbo
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_NORMAL,
							3,
							gl.FLOAT, false, 11 * 4, 6 * 4);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_NORMAL);
	//Texture vbo
	gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0,
							2, //2 is for S and T coordinates
							gl.FLOAT, false, 11 * 4, 9 * 4);
	gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_TEXTURE0);
	
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
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	var projectionMatrix = mat4.create();
	
	mat4.translate(modelMatrix, modelMatrix, [0.0 ,0.0 ,-6.0]);
	mat4.rotateX(modelMatrix ,modelMatrix, degToRad(angleCube));
	mat4.rotateY(modelMatrix ,modelMatrix, degToRad(angleCube));
	mat4.rotateZ(modelMatrix ,modelMatrix, degToRad(angleCube));
	
	mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, projectionMatrix); 
	
	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, projectionMatrix);
	
	if(gbLighting ==  true)
	{
		gl.uniform1i(lightingEnabledUniform, 1);
		gl.uniform3fv(laUniform, lightAmbient);
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(lsUniform, lightSpecular);
		gl.uniform3fv(kaUniform, materialAmbient);
		gl.uniform3fv(kdUniform, materialDiffuse);
		gl.uniform3fv(ksUniform, materialSpecular);
		gl.uniform1f(materialShininessUniform, materialShininess);
		gl.uniform4fv(lightPositionUniform, lightPosition);
	}
	else
	{
		gl.uniform1i(lightingEnabledUniform, 0);
	}
	
	// bind with texture
	gl.bindTexture(gl.TEXTURE_2D, texture_marble);
	gl.uniform1i(uniform_texture0_sampler, 0);
		
	gl.bindVertexArray(vao_interleaved);
	
	gl.drawArrays(gl.TRIANGLE_FAN,0,4);
	gl.drawArrays(gl.TRIANGLE_FAN,4,4);
	gl.drawArrays(gl.TRIANGLE_FAN,8,4);
	gl.drawArrays(gl.TRIANGLE_FAN,12,4);
	gl.drawArrays(gl.TRIANGLE_FAN,16,4);
	gl.drawArrays(gl.TRIANGLE_FAN,20,4);
	
	gl.bindVertexArray(null); 
	
	gl.useProgram(null);
	
	if(gbAnimation == true)
	{
		angleCube=angleCube+1.0;
	
		if(angleCube>=360.0)
			angleCube=0.0;
	}
	
	// animation loop
	requestAnimationFrame(draw, canvas);
}

function uninitialize()
{
	// code
	if(texture_marble)
	{
		gl.deleteTexture(texture_marble);
		texture_marble=0;
	}
	
	if(vao_interleaved)
	{
		gl.deleteVertexArray(vao_interleaved);
		vao_interleaved=null;
	}
	
	if(vbo_interleaved)
	{
		gl.deleteVertexArray(vbo_interleaved);
		vbo_interleaved=null;
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