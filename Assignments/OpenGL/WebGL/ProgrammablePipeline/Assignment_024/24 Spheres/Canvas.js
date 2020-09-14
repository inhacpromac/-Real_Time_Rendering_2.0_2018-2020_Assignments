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

var lightAmbient = [0.5, 0.5, 0.5];
var lightDiffuse = [1.0, 1.0, 1.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = [0.0, 0.0, 1.0, 0.0];

var materialAmbient = [
    [
        // Column 1
        [0.0215, 0.1745, 0.0215],
        [0.135, 0.2225, 0.1575],
        [0.05375, 0.05, 0.06625],
        [0.25, 0.20725, 0.20725],
        [0.1745, 0.01175, 0.01175],
        [0.1, 0.18725, 0.1745]
    ],
    [
        // Column 2
        [0.329412, 0.223529, 0.027451],
        [0.2125, 0.1275, 0.054],
        [0.25, 0.25, 0.25],
        [0.19125, 0.0735, 0.0225],
        [0.24725, 0.1995, 0.0745],
        [0.19225, 0.19225, 0.19225]
    ],
    [
        // Column 3
        [0.0, 0.0, 0.0],
        [0.0, 0.1, 0.06],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0],
        [0.0, 0.0, 0.0]
    ],
    [
        // Column 4
        [0.02, 0.02, 0.02],
        [0.0, 0.05, 0.05],
        [0.0, 0.05, 0.0],
        [0.05, 0.0, 0.0],
        [0.05, 0.05, 0.05],
        [0.05, 0.05, 0.0]
    ]
];

var materialDiffuse = [
    [
        // Column 1
        [0.07568, 0.61424, 0.07568],
        [0.54, 0.89, 0.63],
        [0.18275, 0.17, 0.22525],
        [1.0, 0.829, 0.829],
        [0.61424, 0.04136, 0.04136],
        [0.396, 0.74151, 0.69102]
    ],
    [
        // Column 2
        [0.780392, 0.568627, 0.113725],
        [0.714, 0.4284, 0.18144],
        [0.4, 0.4, 0.4],
        [0.7038, 0.27048, 0.0828],
        [0.75164, 0.60648, 0.22648],
        [0.50754, 0.50754, 0.50754]
    ],
    [
        // Column 3
        [0.01, 0.01, 0.01],
        [0.0, 0.50980392, 0.50980392],
        [0.1, 0.35, 0.1],
        [0.5, 0.0, 0.0],
        [0.55, 0.55, 0.55],
        [0.5, 0.5, 0.0]
    ],
    [
        // Column 4
        [0.01, 0.01, 0.01],
        [0.4, 0.5, 0.5],
        [0.4, 0.5, 0.4],
        [0.5, 0.4, 0.4],
        [0.5, 0.5, 0.5],
        [0.5, 0.5, 0.4]
    ]
];

var materialSpecular = [
    [
        // Column 1
        [0.633, 0.727811, 0.633],
        [0.316228, 0.316228, 0.316228],
        [0.332741, 0.328634, 0.346435],
        [0.296648, 0.296648, 0.296648],
        [0.727811, 0.626959, 0.626959],
        [0.297254, 0.30829, 0.306678]
    ],
    [
        // Column 2
        [0.992157, 0.941176, 0.807843],
        [0.393548, 0.271906, 0.166721],
        [0.774597, 0.774597, 0.774597],
        [0.256777, 0.137622, 0.086014],
        [0.628281, 0.555802, 0.366065],
        [0.508273, 0.508273, 0.508273]
    ],
    [
        // Column 3
        [0.5, 0.5, 0.5],
        [0.50196078, 0.50196078, 0.50196078],
        [0.45, 0.55, 0.45],
        [0.7, 0.6, 0.6],
        [0.7, 0.7, 0.7],
        [0.6, 0.6, 0.5]
    ],
    [
        // Column 4
        [0.4, 0.4, 0.4],
        [0.04, 0.7, 0.7],
        [0.04, 0.7, 0.04],
        [0.7, 0.04, 0.04],
        [0.7, 0.7, 0.7],
        [0.7, 0.7, 0.04]
    ]
];

var materialShininess = [
    [
        // Column 1
        0.6 * 128.0,
        0.1 * 128.0,
        0.3 * 128.0,
        0.088 * 128.0,
        0.6 * 128.0,
        0.1 * 128.0
    ],
    [
        // Column 2
        0.21794872 * 128.0,
        0.2 * 128.0,
        0.6 * 128.0,
        0.1 * 128.0,
        0.4 * 128.0,
        0.4 * 128.0
    ],
    [
        // Column 3
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0,
        0.25 * 128.0
    ],
    [
        // Column 4
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0,
        0.078125 * 128.0
    ]
];

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

var angleLight = 0.0;
var speed = 0.1;

var gbAnimation 	= false;
var gbLighting 		= false;
var lightXRotate 	= true;
var lightYRotate 	= false;
var lightZRotate 	= false;

var totalNumberOfRows = 6;
var totalNumberOfColumns = 4;
var vWidth = 1;
var vHeight = 1;
var vX = 0;
var vY = 0;

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
	laUniform = gl.getUniformLocation(shaderProgramObject, "u_la");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "u_ld");
	lsUniform = gl.getUniformLocation(shaderProgramObject, "u_ls");
	kaUniform = gl.getUniformLocation(shaderProgramObject, "u_ka");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "u_kd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "u_ks");
	materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "u_material_shininess");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "u_light_position");
	lightingEnabledUniform = gl.getUniformLocation(shaderProgramObject, "u_lkeypressed");
	
	
	sphere = new Mesh();
	makeSphere(sphere,2.0,30,30);
	
	
	// set clear color
	gl.clearColor(0.25, 0.25, 0.25, 1.0);

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
	
	vWidth = canvas.width / totalNumberOfRows;
	vHeight = canvas.height / totalNumberOfRows;
	vX = (canvas.width - (vWidth * totalNumberOfColumns)) / 2;
	vY = (canvas.height - (vHeight * totalNumberOfRows)) / 2;
	
	// set the viewport to match
	//gl.viewport(0, 0, canvas.width, canvas.height);
	
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
	
	for (var column = 0; column < totalNumberOfColumns; ++column)
	{
		for (var row = 0; row < totalNumberOfRows; ++row)
		{
			gl.viewport(vX + (column * vWidth), vY + (row * vHeight), vWidth, vHeight);

			gl.useProgram(shaderProgramObject);

			if(gbLighting ==  true)
			{
				gl.uniform1i(lightingEnabledUniform, 1);
				gl.uniform3fv(laUniform, lightAmbient);
				gl.uniform3fv(ldUniform, lightDiffuse);
				gl.uniform3fv(lsUniform, lightSpecular);
				gl.uniform3fv(kaUniform, materialAmbient[column][row]);
				gl.uniform3fv(kdUniform, materialDiffuse[column][row]);
				gl.uniform3fv(ksUniform, materialSpecular[column][row]);
				gl.uniform1f(materialShininessUniform, materialShininess[column][row]);
				
				if (lightXRotate)
				{
					lightPosition[0] = 0.0;
					lightPosition[1] = Math.sin(angleLight) * 100.0 - 3.0;
					lightPosition[2] = Math.cos(angleLight) * 100.0 - 3.0;
				}
				else if (lightYRotate)
				{
					lightPosition[0] = Math.sin(angleLight) * 100.0 - 3.0;
					lightPosition[1] = 0.0;
					lightPosition[2] = Math.cos(angleLight) * 100.0 - 3.0;
				}
				else if (lightZRotate)
				{
					lightPosition[0] = Math.sin(angleLight) * 100.0 - 3.0;
					lightPosition[1] = Math.cos(angleLight) * 100.0 - 3.0;
					lightPosition[2] = 0.0;
				}
				
				gl.uniform4fv(lightPositionUniform, lightPosition);
			}
			else
			{
				gl.uniform1i(lightingEnabledUniform, 0);
			}
			
			var modelMatrix=mat4.create();
			var viewMatrix=mat4.create();
			var projectionMatrix = mat4.create();
			
			mat4.translate(modelMatrix, modelMatrix, [0.0 ,0.0 ,-10.0]);
			
			mat4.multiply(projectionMatrix, perspectiveProjectionMatrix, projectionMatrix); 
			
			gl.uniformMatrix4fv(mUniform, false, modelMatrix);
			gl.uniformMatrix4fv(vUniform, false, viewMatrix);
			gl.uniformMatrix4fv(pUniform, false, projectionMatrix);
				
			sphere.draw();
			
			gl.useProgram(null);

		}
	}
	
	// animation loop
	requestAnimationFrame(draw, canvas);
	
	if (gbAnimation == true)
	{
		angleLight += speed;

		if (angleLight >= 360.0)
		{
			angleLight = 0.0;
		}
	}
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
			gl.devareShader(fragmentShaderObject);
			fragmentShaderObject=null;
		}
		
		if(vertexShaderObject)
		{
			gl.detachShader(shaderProgramObject,vertexShaderObject);
			gl.devareShader(vertexShaderObject);
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
			if(gbAnimation == false)
			{
				gbAnimation = true;
			}
			else
			{
				gbAnimation = false;
			}
		case 67:
			if(lightXRotate == true && lightYRotate == false && lightZRotate == false)
			{
				lightXRotate = false;
				lightYRotate = true;	//Yrotation
				lightZRotate = false;
				//break;
			}
			else if(lightXRotate == false && lightYRotate == true && lightZRotate == false)
			{
				lightXRotate = false;
				lightYRotate = false;
				lightZRotate = true;	//Zrotation
				//break;
			}
			else if(lightXRotate == false && lightYRotate == false && lightZRotate == true)
			{
				lightXRotate = true;	//Xrotation
				lightYRotate = false;
				lightZRotate = false;
				//break;
			}
	}
}

function mouseDown()
{
	// code
}