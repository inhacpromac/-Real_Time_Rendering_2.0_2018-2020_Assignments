//Assignment_023
//Checkerboard

//header
#include <windows.h>
#include <stdio.h> //for file I/O

#include <d3d11.h>
#include <d3dcompiler.h>
#include "WICTextureLoader.h" //for DirectX::CreateWICTextureFromFile 

#pragma warning( disable : 4838 )
#include "XNAMath\xnamath.h"

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "D3dcompiler.lib")
#pragma comment (lib, "DirectXTK.lib ") //for DirectX::CreateWICTextureFromFile

#define WIN_WIDTH 800
#define WIN_HEIGHT 600
#define checkImageWidth 64
#define checkImageHeight 64

using namespace DirectX;

//global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global varibale declarations
FILE* gpFile = NULL;
char gszLogFileName[] = "Log.txt";

HWND ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

float gClearColor[4]; //RGBA
IDXGISwapChain* gpIDXGISwapChain = NULL;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;

ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Texture = NULL;
ID3D11Buffer* gpID3D11Buffer_VertexBuffer_Position = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_ConstantBuffer = NULL;

ID3D11RasterizerState* gpID3D11RasterizerState = NULL;

ID3D11DepthStencilView* gpID3D11DepthStencilView = NULL;

ID3D11ShaderResourceView* gpID3D11ShaderResourceView_Texture = NULL;
ID3D11SamplerState* gpID3D11SamplerState_Texture = NULL;

struct CBUFFER
{
	XMMATRIX WorldViewProjectionMatrix;
};

XMMATRIX gPerspectiveProjectionMatrix;

float angle = 0.0f;
float speed = 1.0f;

//WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpszCmdLine, int iCmdShow)
{
	//function declarations
	HRESULT initialize(void);
	void uninitialize(void);
	void display(void);

	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szClassName[] = TEXT("Direct3D11");
	bool bDone = false;

	//code
	//create log file
	if (fopen_s(&gpFile, gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Log File Can Not Be Created.\nExitting..."), TEXT("Error"), MB_OK | MB_TOPMOST | MB_ICONSTOP);
		exit(0);
	}
	else
	{
		fprintf_s(gpFile, "Log File Is Successfully Opened.\n");
		fclose(gpFile);
	}

	//initialize WNDCLASSEX structure
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	//register WNDCLASSEX structure
	RegisterClassEx(&wndclass);

	//create window
	hwnd = CreateWindow(szClassName,
		TEXT("Direct3D11 INHACPROMAC"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//initialize D3D
	HRESULT hr;
	hr = initialize();
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "initialize() Failed. Exitting Now ...\n");
		fclose(gpFile);
		DestroyWindow(hwnd);
		hwnd = NULL;
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "initialize() Succeeded.\n");
		fclose(gpFile);
	}

	//message loop game loop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			//render
			display();
			if (gbActiveWindow == true)
			{
				if (gbEscapeKeyIsPressed == true)
					bDone = true;
			}
		}
	}

	//clean-up
	uninitialize();

	return((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	//function declarations
	HRESULT resize(int, int);
	void ToggleFullscreen(void);
	void uninitialize(void);

	//variable declaratrions
	HRESULT hr;

	//code
	switch (iMsg)
	{
	case WM_ACTIVATE:
		if (HIWORD(wParam) == 0) //if 0, window is active
			gbActiveWindow = true;
		else //if non-zero, window is not active
			gbActiveWindow = false;
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_SIZE:
		if (gpID3D11DeviceContext) {
			hr = resize(LOWORD(lParam), HIWORD(lParam));
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "resize() Failed.\n");
				fclose(gpFile);
				return(hr);
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "resize() Succeeded.\n");
				fclose(gpFile);
			}
		}
		break;
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE: //case 27
			if (gbEscapeKeyIsPressed == false)
				gbEscapeKeyIsPressed = true;
			break;
		case 0x46: //'F'or'f'
			if (gbFullscreen == false) {
				ToggleFullscreen();
				gbFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = false;
			}
			break;
		default:
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_CLOSE:
		uninitialize();
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	//variable declarations
	MONITORINFO mi;

	//code
	if (gbFullscreen == false)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };

			if (GetWindowPlacement(ghwnd, &wpPrev) &&
				GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd,
					HWND_TOP,
					mi.rcMonitor.left,
					mi.rcMonitor.top,
					mi.rcMonitor.right - mi.rcMonitor.left,
					mi.rcMonitor.bottom - mi.rcMonitor.top,
					SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}
		ShowCursor(FALSE);
	}
	else
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
	}
}

HRESULT initialize(void)
{
	//function declarations
	//HRESULT LoadD3DTexture(const wchar_t*, ID3D11ShaderResourceView**);
	HRESULT CheckerBoardTexture(ID3D11ShaderResourceView**);
	void uninitialize(void);
	HRESULT resize(int, int);

	//variable declarations
	HRESULT hr;
	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0; //default, lowest
	UINT createDeviceFlags = 0;
	UINT numDriverTypes = 0;
	UINT numFeatureLevels = 1; //based upon d3dFeatureLevel_required

	//code
	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);
	//calculating size of array

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		d3dDriverType = d3dDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,							//Adapter
			d3dDriverType,					//Driver Type
			NULL,							//Software
			createDeviceFlags,				//Flags
			&d3dFeatureLevel_required,		//Feature Levels
			numFeatureLevels,				//Num Feature Levels
			D3D11_SDK_VERSION,				//SDK Version
			&dxgiSwapChainDesc,				//SwapChain Desc 
			&gpIDXGISwapChain,				//Swap Chain
			&gpID3D11Device,				//Device
			&d3dFeatureLevel_acquired,		//Feature Level
			&gpID3D11DeviceContext);		//Device Context
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3D11CreateDeviceAndSwapChain() Succeeded.\n");
		fprintf_s(gpFile, "The Choosen Driver is of ");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf_s(gpFile, "Hardware Type.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf_s(gpFile, "Warp Type.\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf_s(gpFile, "Reference Type.\n");
		}
		else
		{
			fprintf_s(gpFile, "Unknown Type.\n");
		}

		fprintf_s(gpFile, "The Supported Highest Feature Level is ");
		if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0) {
			fprintf_s(gpFile, "11.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1) {
			fprintf_s(gpFile, "10.1\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0) {
			fprintf_s(gpFile, "10.0\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_9_3) {
			fprintf_s(gpFile, "9.3\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_9_2) {
			fprintf_s(gpFile, "9.2\n");
		}
		else if (d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_9_1) {
			fprintf_s(gpFile, "9.1\n");
		}
		else
		{
			fprintf_s(gpFile, "Unknown.\n");
		}
		fclose(gpFile);
	}

	//initialize shader, input layouts, constant buffers etc...
	const char* vertexShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"\n" \
		"struct vertex_output" \
		"{" \
		"   float4 position	:	SV_POSITION;" \
		"	float2 texcoord :	TEXCOORD;" \
		"};" \
		"\n" \
		"vertex_output main(float4 pos : POSITION, float2 texcoord : TEXCOORD)" \
		"{" \
		"   vertex_output output;" \
		"   output.position = mul(worldViewProjectionMatrix, pos);" \
		"	output.texcoord = texcoord;" \
		"   return output;" \
		"}";
	ID3DBlob* pID3DBlob_VertexShaderCode = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	hr = D3DCompile(vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Vertex Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() Succeeded For Vertex Shader.\n ");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreateVertexShader(pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateVertexShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateVertexShader() Succeeded. \n ");
		fclose(gpFile);
	}
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, 0, 0);

	const char* pixelShaderSourceCode =
		"Texture2D myTexture2D;" \
		"SamplerState mySamplerState;" \
		"float4 main(float4 position : SV_POSITION, float2 texcoord : TEXCOORD): SV_TARGET" \
		"{" \
		"	float4 color = myTexture2D.Sample(mySamplerState, texcoord);" \
		"   return color;" \
		"}";

	ID3DBlob* pID3DBlob_PixelShaderCode = NULL;
	pID3DBlob_Error = NULL;
	hr = D3DCompile(pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"PS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderCode,
		&pID3DBlob_Error);

	if (FAILED(hr))
	{
		if (pID3DBlob_Error != NULL)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "D3DCompile() Failed For Pixel Shader : %s.\n", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFile);
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
			return(hr);
		}
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "D3DCompile() Succeeded For Pixel Shader.\n ");
		fclose(gpFile);
	}

	hr = gpID3D11Device->CreatePixelShader(pID3DBlob_PixelShaderCode->GetBufferPointer(),
		pID3DBlob_PixelShaderCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreatePixelShader() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreatePixelShader() Succeeded.\n ");
		fclose(gpFile);
	}

	//create and set input layout
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[2];
	inputElementDesc[0].SemanticName = "POSITION";
	inputElementDesc[0].SemanticIndex = 0;
	inputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDesc[0].InputSlot = 0;
	inputElementDesc[0].AlignedByteOffset = 0;
	inputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[0].InstanceDataStepRate = 0;

	inputElementDesc[1].SemanticName = "TEXCOORD";
	inputElementDesc[1].SemanticIndex = 0;
	inputElementDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDesc[1].InputSlot = 1;
	inputElementDesc[1].AlignedByteOffset = 0;
	inputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputElementDesc[1].InstanceDataStepRate = 0;

	hr = gpID3D11Device->CreateInputLayout(inputElementDesc,
		_ARRAYSIZE(inputElementDesc),
		pID3DBlob_VertexShaderCode->GetBufferPointer(),
		pID3DBlob_VertexShaderCode->GetBufferSize(),
		&gpID3D11InputLayout);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateInputLayout() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateInputLayout() Succeeded.\n");
		fclose(gpFile);
	}
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);
	pID3DBlob_VertexShaderCode->Release();
	pID3DBlob_VertexShaderCode = NULL;

	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, 0, 0);
	pID3DBlob_PixelShaderCode->Release();
	pID3DBlob_PixelShaderCode = NULL;

	//float vertices[] =
	//{
	//	// triangle 1
	//	-1.0f, +1.0f, 0.0f,
	//	+1.0f, +1.0f, 0.0f,
	//	-1.0f, -1.0f, 0.0f,
	//	// triangle 2
	//	-1.0f, -1.0f, 0.0f,
	//	+1.0f, +1.0f, 0.0f,
	//	+1.0f, -1.0f, 0.0f
	//};

	//create vertex buffer
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * 18;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_Position);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Position Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Position Buffer.\n");
		fclose(gpFile);
	}

	////copy triangleVertices into above buffer
	//D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position,
	//	NULL,
	//	D3D11_MAP_WRITE_DISCARD,
	//	NULL,
	//	&mappedSubresource);
	//memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
	//gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, NULL);

	float texture[] =
	{
		//triangle1
		+0.0f, +0.0f,
		+1.0f, +0.0f,
		+0.0f, +1.0f,
		//triangle2
		+0.0f, +1.0f,
		+1.0f, +0.0f,
		+1.0f, +1.0f
	};

	//create vertex buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(float) * ARRAYSIZE(texture);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_VertexBuffer_Texture);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Failed For Vertex Texture Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateBuffer() Succeeded For Vertex Texture Buffer.\n");
		fclose(gpFile);
	}

	//copy triangleVertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Texture,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);
	memcpy(mappedSubresource.pData, texture, sizeof(texture));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Texture, NULL);

	//define and set the constant buffer
	D3D11_BUFFER_DESC bufferDesc_ConstantBuffer;
	ZeroMemory(&bufferDesc_ConstantBuffer, sizeof(D3D11_BUFFER_DESC));
	bufferDesc_ConstantBuffer.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc_ConstantBuffer.ByteWidth = sizeof(CBUFFER);
	bufferDesc_ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = gpID3D11Device->CreateBuffer(&bufferDesc_ConstantBuffer,
		nullptr,
		&gpID3D11Buffer_ConstantBuffer);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateInputLayout() Failed For Constant Buffer.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateInputLayout() Succeeded For Constant Buffer.\n");
		fclose(gpFile);
	}
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_ConstantBuffer);

	//set rasterization state
	//In D3D, backface culling is by default ON
	//Means backface of geometry will not be visible
	//so set culling OFF
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory((void*)&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_NONE; //this allows both sides of 2D geometry visible on rotation
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	hr = gpID3D11Device->CreateRasterizerState(&rasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRasterizerState() Failed For Culling.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRasterizerState() Succeeded For Culling.\n");
		fclose(gpFile);
	}
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	////create texture resource and texture view
	//hr = LoadD3DTexture(L"marble.bmp", &gpID3D11ShaderResourceView_Texture);
	//if (FAILED(hr))
	//{
	//	fopen_s(&gpFile, gszLogFileName, "a+");
	//	fprintf_s(gpFile, "LoadD3DTexture() Failed For  Texture.\n");
	//	fclose(gpFile);
	//	return(hr);
	//}
	//else
	//{
	//	fopen_s(&gpFile, gszLogFileName, "a+");
	//	fprintf_s(gpFile, "LoadD3DTexture() Succeeded For  Texture.\n ");
	//	fclose(gpFile);
	//}
	////create the sampler state
	//D3D11_SAMPLER_DESC samplerDesc;
	//ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//hr = gpID3D11Device->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState_Texture);
	//if (FAILED(hr))
	//{
	//	fopen_s(&gpFile, gszLogFileName, "a+");
	//	fprintf_s(gpFile, "ID3D11Device::CreateSamplerState() Failed For Texture.\n");
	//	fclose(gpFile);
	//	return(hr);
	//}
	//else
	//{
	//	fopen_s(&gpFile, gszLogFileName, "a+");
	//	fprintf_s(gpFile, "ID3D11Device::CreateSamplerState() Succeeded For Texture.\n");
	//	fclose(gpFile);
	//}

	hr = CheckerBoardTexture(&gpID3D11ShaderResourceView_Texture);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CheckerBoardTexture() Failed For  Texture.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CheckerBoardTexture() Succeeded For  Texture.\n ");
		fclose(gpFile);
	}
	//create the sampler state
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	hr = gpID3D11Device->CreateSamplerState(&samplerDesc, &gpID3D11SamplerState_Texture);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateSamplerState() Failed For Texture.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateSamplerState() Succeeded For Texture.\n");
		fclose(gpFile);
	}



	//d3dclearColor
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 0.0f;
	gClearColor[3] = 1.0f;

	//set projection matrix to identity matrix
	gPerspectiveProjectionMatrix = XMMatrixIdentity();

	//call resize for first time
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "resize() Succeeded.\n");
		fclose(gpFile);
	}

	return(S_OK);
}

//ID3D11Resource* textureTemp = NULL;

//HRESULT CheckerBoardTexture(ID3D11ShaderResourceView** ppID3D11ShaderResourceView)
//{
//	HRESULT hr;
//
//
//
//
//	// Allocate temporary memory for image
//	size_t rowPitch = (64 * 64 + 7) / 8;
//	size_t imageSize = rowPitch * 64;
//
//	uint8_t *temp = (new uint8_t[imageSize]);
//
//
//
//
//	byte checkImage[checkImageHeight][checkImageWidth][4];
//	memset((void*)checkImage, 0, sizeof(checkImage));
//
//	int i, j, c;
//
//	for (i = 0; i < checkImageHeight; ++i)
//	{
//		for (j = 0; j < checkImageWidth; ++j)
//		{
//			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
//			checkImage[i][j][0] = (byte)c;
//			checkImage[i][j][1] = (byte)c;
//			checkImage[i][j][2] = (byte)c;
//			checkImage[i][j][3] = (byte)255;
//		}
//	}
//
//
//
//
//	// Create texture
//	D3D11_TEXTURE2D_DESC checkerBoardDesc;
//	ZeroMemory(&checkerBoardDesc, sizeof(D3D11_TEXTURE2D_DESC));
//	checkerBoardDesc.Width = checkImageWidth;
//	checkerBoardDesc.Height = checkImageHeight;
//	checkerBoardDesc.MipLevels = 1;
//	checkerBoardDesc.ArraySize = 1;
//	checkerBoardDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
//	checkerBoardDesc.SampleDesc.Count = 1;
//	checkerBoardDesc.SampleDesc.Quality = 0;
//	checkerBoardDesc.Usage = D3D11_USAGE_DEFAULT;
//	checkerBoardDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
//	checkerBoardDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 0
//	checkerBoardDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS; // 0
//
//	D3D11_SUBRESOURCE_DATA initData;
//	initData.pSysMem = temp;
//	initData.SysMemPitch = static_cast<UINT>(rowPitch);
//	initData.SysMemSlicePitch = static_cast<UINT>(imageSize);
//
//	ID3D11Texture2D* texCheckerBoard;
//
//	hr = gpID3D11Device->CreateTexture2D(&checkerBoardDesc, &initData, &texCheckerBoard);
//	if (SUCCEEDED(hr) && texCheckerBoard != 0)
//	{
//		if (ppID3D11ShaderResourceView != 0)
//		{
//			D3D11_SHADER_RESOURCE_VIEW_DESC checkerBoardSRVDesc;
//			memset(&checkerBoardSRVDesc, 0, sizeof(checkerBoardSRVDesc));
//			checkerBoardSRVDesc.Format = checkerBoardDesc.Format;
//			checkerBoardSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//			checkerBoardSRVDesc.Texture2D.MostDetailedMip = 0;
//			checkerBoardSRVDesc.Texture2D.MipLevels = 1;
//
//			hr = gpID3D11Device->CreateShaderResourceView(texCheckerBoard, &checkerBoardSRVDesc, ppID3D11ShaderResourceView);
//			if (FAILED(hr))
//			{
//				fopen_s(&gpFile, gszLogFileName, "a+");
//				fprintf_s(gpFile, "CreateShaderResourceView() Failed.\n");
//				fclose(gpFile);
//				return(hr);
//			}
//			else
//			{
//				fopen_s(&gpFile, gszLogFileName, "a+");
//				fprintf_s(gpFile, "CreateShaderResourceView() Succeeded.\n");
//				fclose(gpFile);
//			}
//
//			//gpID3D11DeviceContext->UpdateSubresource(texCheckerBoard, 0, nullptr, temp, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize));
//			//gpID3D11DeviceContext->GenerateMips(*ppID3D11ShaderResourceView);
//		}
//
//		hr = DirectX::CreateWICTextureFromMemory(gpID3D11Device,
//			gpID3D11DeviceContext,
//			(const uint8_t*)checkImage,
//			sizeof(checkImage),
//			NULL,
//			ppID3D11ShaderResourceView
//		);
//
//		//hr = DirectX::CreateWICTextureFromMemoryEx(gpID3D11Device,
//		//	(uint8_t*)checkImage,
//		//	sizeof(checkImage),
//		//	sizeof(checkImage),
//		//	D3D11_USAGE_DEFAULT,
//		//	D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
//		//	D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ,
//		//	D3D11_RESOURCE_MISC_GENERATE_MIPS,
//		//	0/*DirectX::WIC_LOADER_FORCE_RGBA32*/,
//		//	NULL,
//		//	ppID3D11ShaderResourceView
//		//);
// 		if (FAILED(hr))
//		{
//			fopen_s(&gpFile, gszLogFileName, "a+");
//			fprintf_s(gpFile, "CreateWICTextureFromMemory() Failed.\n");
//			fclose(gpFile);
//			return(hr);
//		}
//		else
//		{
//			fopen_s(&gpFile, gszLogFileName, "a+");
//			fprintf_s(gpFile, "CreateWICTextureFromMemory() Succeeded.\n");
//			fclose(gpFile);
//		}
//		
//	}
//
//	return hr;
//}


HRESULT CheckerBoardTexture(ID3D11ShaderResourceView** ppID3D11ShaderResourceView)
{
	HRESULT hr;



	// Allocate temporary memory for image
	//size_t rowPitch = (64 * 64 + 7) / 8;
	//size_t imageSize = rowPitch * 64;
	
	//uint8_t *temp = (new uint8_t[imageSize]);


	byte checkImage[checkImageHeight][checkImageWidth][4];
	memset((void*)checkImage, 0, sizeof(checkImage));

	int i, j, c;

	for (i = 0; i < checkImageHeight; ++i)
	{
		for (j = 0; j < checkImageWidth; ++j)
		{
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			checkImage[i][j][0] = (byte)c;
			checkImage[i][j][1] = (byte)c;
			checkImage[i][j][2] = (byte)c;
			checkImage[i][j][3] = (byte)255;
		}
	}



	// Create texture
	D3D11_TEXTURE2D_DESC checkerBoardDesc;
	ZeroMemory(&checkerBoardDesc, sizeof(D3D11_TEXTURE2D_DESC));
	checkerBoardDesc.Width = checkImageWidth;
	checkerBoardDesc.Height = checkImageHeight;
	checkerBoardDesc.MipLevels = 1;
	checkerBoardDesc.ArraySize = 1;
	checkerBoardDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	checkerBoardDesc.SampleDesc.Count = 1;
	checkerBoardDesc.SampleDesc.Quality = 0;
	checkerBoardDesc.Usage = D3D11_USAGE_DEFAULT;
	checkerBoardDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	checkerBoardDesc.CPUAccessFlags = 0;
	checkerBoardDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = checkImage;
	initData.SysMemPitch = (64 * 32 + 7) / 8;
	initData.SysMemSlicePitch = ((64 * 32 + 7) / 8) * 64;

	ID3D11Texture2D* texCheckerBoard;
	hr = gpID3D11Device->CreateTexture2D(&checkerBoardDesc, &initData, &texCheckerBoard);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateTexture2D() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		/*if (!texCheckerBoard)
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "CreateShaderResourceView() Failed.\n");
			fclose(gpFile);
			return(hr);
		}
		else
		{
			fopen_s(&gpFile, gszLogFileName, "a+");
			fprintf_s(gpFile, "CreateShaderResourceView() Succeeded.\n");
			fclose(gpFile);
		}*/

		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "CreateTexture2D() Succeeded.\n");
		fclose(gpFile);
	}


	if (SUCCEEDED(hr) && texCheckerBoard != 0)
	{
		if (ppID3D11ShaderResourceView != 0)
		{
			/*D3D11_RENDER_TARGET_VIEW_DESC checkerBoardRTDesc;
			checkerBoardRTDesc.Format = checkerBoardDesc.Format;
			checkerBoardRTDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			checkerBoardRTDesc.Texture2D.MipSlice = 0;

			ID3D11RenderTargetView* ID3D11RenderTargetView_CheckerBoardTexture = NULL;
			hr = gpID3D11Device->CreateRenderTargetView(texCheckerBoard, &checkerBoardRTDesc, &ID3D11RenderTargetView_CheckerBoardTexture);
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "CreateRenderTargetView() Failed.\n");
				fclose(gpFile);
				return(hr);
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "CreateRenderTargetView() Succeeded.\n");
				fclose(gpFile);
			}*/

			D3D11_SHADER_RESOURCE_VIEW_DESC checkerBoardSRVDesc;
			memset(&checkerBoardSRVDesc, 0, sizeof(checkerBoardSRVDesc));
			checkerBoardSRVDesc.Format = checkerBoardDesc.Format;
			checkerBoardSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			checkerBoardSRVDesc.Texture2D.MostDetailedMip = 0;
			checkerBoardSRVDesc.Texture2D.MipLevels = 1;

			hr = gpID3D11Device->CreateShaderResourceView(texCheckerBoard, &checkerBoardSRVDesc, ppID3D11ShaderResourceView);
			if (FAILED(hr))
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "CreateShaderResourceView() Failed.\n");
				fclose(gpFile);
				return(hr);
			}
			else
			{
				fopen_s(&gpFile, gszLogFileName, "a+");
				fprintf_s(gpFile, "CreateShaderResourceView() Succeeded.\n");
				fclose(gpFile);
			}

			/*gpID3D11DeviceContext->UpdateSubresource(texCheckerBoard, 0, nullptr, temp, static_cast<UINT>(rowPitch), static_cast<UINT>(imageSize));
			gpID3D11DeviceContext->GenerateMips(*ppID3D11ShaderResourceView);*/

		}


	//	hr = DirectX::CreateWICTextureFromMemory(gpID3D11Device,
	//		gpID3D11DeviceContext,
	//		(const uint8_t*)texCheckerBoard,
	//		sizeof(texCheckerBoard),
	//		NULL,
	//		ppID3D11ShaderResourceView
	//	);
	//	if (FAILED(hr))
	//	{
	//		fopen_s(&gpFile, gszLogFileName, "a+");
	//		fprintf_s(gpFile, "CreateWICTextureFromMemory() Failed.\n");
	//		fclose(gpFile);
	//		return(hr);
	//	}
	//	else
	//	{
	//		fopen_s(&gpFile, gszLogFileName, "a+");
	//		fprintf_s(gpFile, "CreateWICTextureFromMemory() Succeeded.\n");
	//		fclose(gpFile);
	//	}
	}
	return hr;
}



	////create the render target texture for checkboard
	//D3D11_TEXTURE2D_DESC checkerBoardDesc;
	//ZeroMemory(&checkerBoardDesc, sizeof(D3D11_TEXTURE2D_DESC));
	//checkerBoardDesc.Width = 256;
	//checkerBoardDesc.Height = 256;
	//checkerBoardDesc.MipLevels = 1;
	//checkerBoardDesc.ArraySize = 1;
	//checkerBoardDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//checkerBoardDesc.SampleDesc.Count = 1;
	//checkerBoardDesc.SampleDesc.Quality = 0;
	//checkerBoardDesc.Usage = D3D11_USAGE_DEFAULT;
	//checkerBoardDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//checkerBoardDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//ID3D11Texture2D* pID3D11Texture2D_CheckerBoardRenderTargetTexture = NULL;
	//gpID3D11Device->CreateTexture2D(&checkerBoardDesc, NULL, &pID3D11Texture2D_CheckerBoardRenderTargetTexture);



	////create the render target texture view for checkboard
	//D3D11_RENDER_TARGET_VIEW_DESC rtcheckerBoardDesc;
	//rtcheckerBoardDesc.Format = checkerBoardDesc.Format;
	//rtcheckerBoardDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	//rtcheckerBoardDesc.Texture2D.MipSlice = 0;

	//ID3D11RenderTargetView* ID3D11RenderTargetView_CheckerBoardTexture = NULL;
	//gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_CheckerBoardRenderTargetTexture, &rtcheckerBoardDesc, &ID3D11RenderTargetView_CheckerBoardTexture);



	////create the shader-resource view
	//D3D11_SHADER_RESOURCE_VIEW_DESC srcheckerBoardDesc;
	//srcheckerBoardDesc.Format = checkerBoardDesc.Format;
	//srcheckerBoardDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//srcheckerBoardDesc.Texture2D.MostDetailedMip = 0;
	//srcheckerBoardDesc.Texture2D.MipLevels = 1;

	//ID3D11ShaderResourceView* ID3D11ShaderResourceView_CheckerBoardView = NULL;
	//gpID3D11Device->CreateShaderResourceView(pID3D11Texture2D_CheckerBoardRenderTargetTexture, &srcheckerBoardDesc, &ID3D11ShaderResourceView_CheckerBoardView);




	//D3D10_MAPPED_TEXTURE2D mappedTex;
	//pID3D11Texture2D_CheckerBoardRenderTargetTexture->Map(D3D11CalcSubresource(0, 0, 1), D3D11_MAP_WRITE_DISCARD, 0, &mappedTex);

	//UCHAR* pTexels = (UCHAR*)mappedTex.pData;
	//for (UINT row = 0; row < desc.Height; row++r4)
	//{
	//	UINT rowStart = row * mappedTex.RowPitch;
	//	for (UINT col = 0; col < desc.Width; col++)
	//	{
	//		UINT colStart = col * 4;
	//		pTexels[rowStart + colStart + 0] = 255; // Red
	//		pTexels[rowStart + colStart + 1] = 128; // Green
	//		pTexels[rowStart + colStart + 2] = 64;  // Blue
	//		pTexels[rowStart + colStart + 3] = 32;  // Alpha
	//	}
	//}

	//pID3D11Texture2D_CheckerBoardRenderTargetTexture->Unmap(D3D11CalcSubresource(0, 0, 1));
	

//HRESULT LoadD3DTexture(const wchar_t* textureFileName, ID3D11ShaderResourceView** ppID3D11ShaderResourceView)
//{
//	// code
//	HRESULT hr;
//	// create texture
//	hr = DirectX::CreateWICTextureFromFile(gpID3D11Device,
//		gpID3D11DeviceContext,
//		textureFileName,
//		NULL,
//		ppID3D11ShaderResourceView);
//	if (FAILED(hr))
//	{
//		fopen_s(&gpFile, gszLogFileName, "a+");
//		fprintf_s(gpFile, "CreateWICTextureFromFile() Failed For Texture Resource.\n");
//		fclose(gpFile);
//		return(hr);
//	}
//	else
//	{
//		fopen_s(&gpFile, gszLogFileName, "a+");
//		fprintf_s(gpFile, "CreateWICTextureFromFile() Succeeded For Texture Resource.\n");
//		fclose(gpFile);
//	}
//
//	return(hr);
//}


HRESULT resize(int width, int height)
{
	//code
	HRESULT hr = S_OK;

	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	//resize swap chain buffers accordingly
	gpIDXGISwapChain->ResizeBuffers(1,
		width,
		height,
		DXGI_FORMAT_R8G8B8A8_UNORM,
		0);

	//again get back buffer from swapchain
	ID3D11Texture2D* pID3D11Texture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0,
		_uuidof(ID3D11Texture2D),
		(LPVOID*)&pID3D11Texture2D_BackBuffer);

	//again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D_BackBuffer,
		NULL,
		&gpID3D11RenderTargetView);

	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() Succeeded.");
		fprintf_s(gpFile, "With Width:%d and Height:%d.\n", width, height);
		fclose(gpFile);
	}
	pID3D11Texture2D_BackBuffer->Release();
	pID3D11Texture2D_BackBuffer = NULL;

	//create depth stencil buffer (or zbuffer)
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)width;
	textureDesc.Height = (UINT)height;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1; //in real world, this can be up to 4
	textureDesc.SampleDesc.Quality = 0; //if above is 4, then it is 1
	textureDesc.Format = DXGI_FORMAT_D32_FLOAT;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	ID3D11Texture2D* pID3D11Texture2D_DepthBuffer;
	gpID3D11Device->CreateTexture2D(&textureDesc, NULL, &pID3D11Texture2D_DepthBuffer);

	//create depth stencil view from above depth stencilbuffer
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_DepthBuffer,
		&depthStencilViewDesc,
		&gpID3D11DepthStencilView);
	if (FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateDepthStencilView() Failed.\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateDepthStencilView() Succeeded.\n");
		fclose(gpFile);
	}
	pID3D11Texture2D_DepthBuffer->Release();
	pID3D11Texture2D_DepthBuffer = NULL;

	//set render target view as render target
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

	//Set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	//set perspective matrix

	gPerspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f),
		float(width) /
		float(height),
		0.1f,
		100.0f);

	return(hr);
}

void display(void)
{
	//code
	//clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);

	//clear the depth/stencil view to 1.0
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Regular rectangle
	float vertices[] =
	{
		-2.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		-2.0f, -1.0f, 0.0f,
		-2.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f
	};

	//copy triangleVertices into above buffer
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);
	memcpy(mappedSubresource.pData, vertices, sizeof(vertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, NULL);

	//select which vertex buffer position to display
	UINT stride = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position,
		&stride, &offset);

	//select which vertex buffer texture to display
	stride = sizeof(float) * 2;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Texture,
		&stride, &offset);

	//select geometry primtive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//bind texture and sampler as pixel shader resource
	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

	//translation is concerned with world matrix transformation
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.6f);
	//rotationMatrix = XMMatrixRotationX(angle);
	//rotationMatrix *= XMMatrixRotationY(angle);
	//rotationMatrix *= XMMatrixRotationZ(angle);
	//scaleMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);

	worldMatrix = scaleMatrix * rotationMatrix * translationMatrix; // ORDER IS VERY... VERY... VERY ... IMPORTANT

	//final WorldViewProjection matrix
	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * gPerspectiveProjectionMatrix;

	//load the data into the constant buffer
	CBUFFER constantBuffer;
	constantBuffer.WorldViewProjectionMatrix = wvpMatrix;
	gpID3D11DeviceContext->UpdateSubresource(
		gpID3D11Buffer_ConstantBuffer, 0, NULL, &constantBuffer, 0, 0);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(6, 0);

	//Regular tilted rectangle
	float tiltedVertices[] =
	{
		1.0f, 1.0f, 0.0f,
		2.41421f, 1.0f, 1.41421f,
		1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		2.41421f, 1.0f, 1.41421f,
		2.41421f, -1.0f, 1.41421f
	};

	//copy triangleVertices into above buffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	gpID3D11DeviceContext->Map(gpID3D11Buffer_VertexBuffer_Position,
		NULL,
		D3D11_MAP_WRITE_DISCARD,
		NULL,
		&mappedSubresource);
	memcpy(mappedSubresource.pData, tiltedVertices, sizeof(tiltedVertices));
	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_VertexBuffer_Position, NULL);

	//select which vertex buffer position to display
	stride = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(0, 1, &gpID3D11Buffer_VertexBuffer_Position,
		&stride, &offset);

	//select which vertex buffer texture to display
	stride = sizeof(float) * 2;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(1, 1, &gpID3D11Buffer_VertexBuffer_Texture,
		&stride, &offset);

	//select geometry primtive
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//bind texture and sampler as pixel shader resource
	gpID3D11DeviceContext->PSSetShaderResources(0, 1, &gpID3D11ShaderResourceView_Texture);
	gpID3D11DeviceContext->PSSetSamplers(0, 1, &gpID3D11SamplerState_Texture);

	// draw vertex buffer to render target
	gpID3D11DeviceContext->Draw(6, 0);

	//switch between front & back buffers
	gpIDXGISwapChain->Present(0, 0);
}
void uninitialize(void)
{
	//code
	if (gpID3D11SamplerState_Texture)
	{
		gpID3D11SamplerState_Texture->Release();
		gpID3D11SamplerState_Texture = NULL;
	}
	if (gpID3D11ShaderResourceView_Texture)
	{
		gpID3D11ShaderResourceView_Texture->Release();
		gpID3D11ShaderResourceView_Texture = NULL;
	}
	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}
	if (gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}
	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}
	if (gpID3D11Buffer_VertexBuffer_Texture)
	{
		gpID3D11Buffer_VertexBuffer_Texture->Release();
		gpID3D11Buffer_VertexBuffer_Texture = NULL;
	}
	if (gpID3D11Buffer_VertexBuffer_Position)
	{
		gpID3D11Buffer_VertexBuffer_Position->Release();
		gpID3D11Buffer_VertexBuffer_Position = NULL;
	}
	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}
	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}
	if (gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	if (gpFile)
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "uninitialize() Succeeded.\n");
		fprintf_s(gpFile, "Log File is Successfully Closed.\n ");
		fclose(gpFile);
	}
}