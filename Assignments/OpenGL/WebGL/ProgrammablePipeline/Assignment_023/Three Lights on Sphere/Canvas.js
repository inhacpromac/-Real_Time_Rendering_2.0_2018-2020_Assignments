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

var lightZeroAmbient = [0.0, 0.0, 0.0];
var lightZeroDiffuse = [1.0, 0.0, 0.0];
var lightZeroSpecular = [1.0, 0.0, 0.0];
var lightZeroPosition = [0.0, 0.0, 0.0, 0.0];

var lightOneAmbient = [0.0, 0.0, 0.0];
var lightOneDiffuse = [0.0, 1.0, 0.0];
var lightOneSpecular = [0.0, 1.0, 0.0];
var lightOnePosition = [0.0, 0.0, 0.0, 0.0];

var lightTwoAmbient = [0.0, 0.0, 0.0];
var lightTwoDiffuse = [0.0, 0.0, 1.0];
var lightTwoSpecular = [0.0, 0.0, 1.0];
var lightTwoPosition = [0.0, 0.0, 0.0, 0.0];

var materialAmbient = [0.0, 0.0, 0.0];
var materialDiffuse = [1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0];
var materialShininess = 128.0; //50.0

var laZeroUniform = 0;
var ldZeroUniform = 0;
var lsZeroUniform = 0;
var laOneUniform = 0;
var ldOneUniform = 0;
var lsOneUniform = 0;
var laTwoUniform = 0;
var ldTwoUniform = 0;
var lsTwoUniform = 0;
var kaUniform = 0;
var kdUniform = 0;
var ksUniform = 0;
var materialShininessUniform = 0;
var lightZeroPositionUniform = 0;
var lightOnePositionUniform = 0;
var lightTwoPositionUniform = 0;
var lightingEnabledUniform = 0;

var angleOfLightZero = 0.0;
var angleOfLightOne = 0.0;
var angleOfLightTwo = 0.0;
var speed = 1.0;

var gbAnimation = false;
var gbLighting 	= false;

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
		"uniform vec4 u_light_position_zero;" +
		"uniform vec4 u_light_position_one;" +
		"uniform vec4 u_light_position_two;" +
		"out vec3 tNorm;" +
		"out vec3 light_direction_zero;" +
		"out vec3 light_direction_one;" +
		"out vec3 light_direction_two;" +
		"out vec3 viewer_vector;" +
		"void main(void)" +
		"{" +
		"   if(u_lkeypressed == 1)" +
		"   {" +
		"       vec4 eye_coordinates = u_v_matrix * u_m_matrix * vPosition;" +
		"       tNorm = mat3(u_v_matrix * u_m_matrix) * vNormal;" +
		"       light_direction_zero = vec3(u_light_position_zero - eye_coordinates);" +
		"       light_direction_one = vec3(u_light_position_one - eye_coordinates);" +
		"       light_direction_two = vec3(u_light_position_two - eye_coordinates);" +
		"       viewer_vector = -eye_coordinates.xyz;" +
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
		"in vec3 tNorm;" +
		"in vec3 light_direction_zero;" +
		"in vec3 light_direction_one;" +
		"in vec3 light_direction_two;" +
		"in vec3 viewer_vector;" +
		"uniform mediump int u_lkeypressed;" +
		"uniform vec3 u_la_zero;" +
		"uniform vec3 u_ld_zero;" +
		"uniform vec3 u_ls_zero;" +
		"uniform vec3 u_la_one;" +
		"uniform vec3 u_ld_one;" +
		"uniform vec3 u_ls_one;" +
		"uniform vec3 u_la_two;" +
		"uniform vec3 u_ld_two;" +
		"uniform vec3 u_ls_two;" +
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
		"       vec3 normalized_viewer_vector = normalize(viewer_vector);" +
		"		\n" +
		"       vec3 normalized_light_direction_zero = normalize(light_direction_zero);" +
		"       float tn_dot_ld_zero = max(dot(normalized_light_direction_zero, normalized_tNorm), 0.0);" +
		"       vec3 ambient_zero = u_la_zero * u_ka;" +
		"       vec3 diffuse_zero = u_ld_zero * u_kd * tn_dot_ld_zero;" +
		"       vec3 reflection_vector_zero = reflect(-normalized_light_direction_zero, normalized_tNorm);" +
		"       vec3 specular_zero = u_ls_zero * u_ks * pow(max(dot(reflection_vector_zero, normalized_light_direction_zero), 0.0), u_material_shininess);" +
		"		\n" +
		"       vec3 normalized_light_direction_one = normalize(light_direction_one);" +
		"       float tn_dot_ld_one = max(dot(normalized_light_direction_one, normalized_tNorm), 0.0);" +
		"       vec3 ambient_one = u_la_one * u_ka;" +
		"       vec3 diffuse_one = u_ld_one * u_kd * tn_dot_ld_one;" +
		"       vec3 reflection_vector_one = reflect(-normalized_light_direction_one, normalized_tNorm);" +
		"       vec3 specular_one = u_ls_one * u_ks * pow(max(dot(reflection_vector_one, normalized_light_direction_one), 0.0), u_material_shininess);" +
		"		\n" +
		"       vec3 normalized_light_direction_two = normalize(light_direction_two);" +
		"       float tn_dot_ld_two = max(dot(normalized_light_direction_two, normalized_tNorm), 0.0);" +
		"       vec3 ambient_two = u_la_two * u_ka;" +
		"       vec3 diffuse_two = u_ld_two * u_kd * tn_dot_ld_two;" +
		"       vec3 reflection_vector_two = reflect(-normalized_light_direction_two, normalized_tNorm);" +
		"       vec3 specular_two = u_ls_two * u_ks * pow(max(dot(reflection_vector_two, normalized_light_direction_two), 0.0), u_material_shininess);" +
		"		\n" +
		"       phong_ads_light = ambient_zero + diffuse_zero + specular_zero + ambient_one + diffuse_one + specular_one + ambient_two + diffuse_two + specular_two;" +
		"   }" +
		"   else" +
		"   {" +
		"       phong_ads_light = vec3(1.0, 1.0, 1.0);" +
		"   }" +
		"	FragColor = vec4(phong_ads_light, 1.0);" +
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
	laTwoUniform = gl.getUniformLocation(shaderProgramObject, "u_la_two");
	ldTwoUniform = gl.getUniformLocation(shaderProgramObject, "u_ld_two");
	lsTwoUniform = gl.getUniformLocation(shaderProgramObject, "u_ls_two");
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");
	lightZeroPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position_zero");
	lightOnePositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position_one");
	lightTwoPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position_two");
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

	if(gbLighting ==  true)
	{
		gl.uniform1i(lightingEnabledUniform, 1);
		gl.uniform3fv(laZeroUniform, lightZeroAmbient);
		gl.uniform3fv(ldZeroUniform, lightZeroDiffuse);
		gl.uniform3fv(lsZeroUniform, lightZeroSpecular);
		gl.uniform3fv(laOneUniform, lightOneAmbient);
		gl.uniform3fv(ldOneUniform, lightOneDiffuse);
		gl.uniform3fv(lsOneUniform, lightOneSpecular);
		gl.uniform3fv(laTwoUniform, lightTwoAmbient);
		gl.uniform3fv(ldTwoUniform, lightTwoDiffuse);
		gl.uniform3fv(lsTwoUniform, lightTwoSpecular);
		gl.uniform3fv(kaUniform, materialAmbient);
		gl.uniform3fv(kdUniform, materialDiffuse);
		gl.uniform3fv(ksUniform, materialSpecular);
		gl.uniform1f(materialShininessUniform, materialShininess);
		gl.uniform4fv(lightZeroPositionUniform, lightZeroPosition);
		gl.uniform4fv(lightOnePositionUniform, lightOnePosition);
		gl.uniform4fv(lightTwoPositionUniform, lightTwoPosition);
	}
	else
	{
		gl.uniform1i(lightingEnabledUniform, 0);
	}
	
	var modelMatrix=mat4.create();
	var viewMatrix=mat4.create();
	var projectionMatrix = mat4.create();
	
	mat4.translate(modelMatrix, modelMatrix, [0.0 ,0.0 ,-6.0]);
	
	mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, projectionMatrix); 
	
	gl.uniformMatrix4fv(mUniform, false, modelMatrix);
	gl.uniformMatrix4fv(vUniform, false, viewMatrix);
	gl.uniformMatrix4fv(pUniform, false, projectionMatrix);
		
	sphere.draw();
	
	gl.useProgram(null);
	
	if (gbAnimation == true)
	{
		angleOfLightZero += speed;
		angleOfLightOne += speed;
		angleOfLightTwo += speed;

		if (angleOfLightZero >= 360.0)
		{
			angleOfLightZero = 0.0;
		}
		lightZeroPosition[0] = 0.0;
		lightZeroPosition[1] = Math.sin(angleOfLightZero * VDG_PI / 180) * 100.0;
		lightZeroPosition[2] = Math.cos(angleOfLightZero * VDG_PI / 180) * 100.0 - 5.0;

		if (angleOfLightOne >= 360.0)
		{
			angleOfLightOne = 0.0;
		}
		lightOnePosition[0] = Math.sin(angleOfLightOne * VDG_PI / 180) * 100.0;
		lightOnePosition[1] = 0.0;
		lightOnePosition[2] = Math.cos(angleOfLightOne * VDG_PI / 180) * 100.0 - 5.0;

		if (angleOfLightTwo >= 360.0)
		{
			angleOfLightTwo = 0.0;
		}
		lightTwoPosition[0] = Math.cos(angleOfLightTwo * VDG_PI / 180) * 100.0 -5.0;
		lightTwoPosition[1] = Math.sin(angleOfLightTwo * VDG_PI / 180) * 100.0;
		lightTwoPosition[2] = 0.0;
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
	
	if(shaderProgramObject)
	{
		if(fragmentShaderObject)
		{
			gl.detachShader(shaderProgramObject,fragmentShaderObject);
			gl.detachShader(fragmentShaderObject);
			fragmentShaderObject=null;
		}
		
		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject,vertexShaderObject);
			gl.detachShader(vertexShaderObject);
			vertexShaderObject=null;
		}
		
		gl.devareProgram(shaderProgramObject);
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
			if(gbLighting == true)
			{
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
}

function mouseDown()
{
	// code
}