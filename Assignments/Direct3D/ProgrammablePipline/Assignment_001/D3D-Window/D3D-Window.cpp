//Assignment_001
//D3D-Window

//header
#include <windows.h>
#include <stdio.h> //for file I/O

#include <d3d11.h>

#pragma comment (lib, "d3d11.lib")

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

//global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//global varibale declarations
FILE *gpFile = NULL;
char gszLogFileName[] = "Log.txt";

HWND ghwnd = NULL;

DWORD dwStyle;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT) };

bool gbActiveWindow = false;
bool gbEscapeKeyIsPressed = false;
bool gbFullscreen = false;

float gClearColor[4]; //RGBA
IDXGISwapChain * gpIDXGISwapChain = NULL;
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;

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
	wndclass.cbClsExtra = 0; wndclass.cbWndExtra = 0; wndclass.lpfnWndProc = WndProc;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION); wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szClassName; wndclass.lpszMenuName = NULL;
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
		fclose(gpFile); DestroyWindow(hwnd); hwnd = NULL;
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
			if (msg.message == WM_QUIT) bDone = true;
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
			if (gbActiveWindow == true) {
				if (gbEscapeKeyIsPressed == true) bDone = true;
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
					fopen_s(&gpFile, gszLogFileName, "a+"); fprintf_s(gpFile, "resize() Failed.\n"); fclose(gpFile);
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
				if (gbEscapeKeyIsPressed == false) gbEscapeKeyIsPressed = true;
				break;
			case 0x46: //'F'or'f'
				if (gbFullscreen == false) {
					ToggleFullscreen(); gbFullscreen = true;
				}
				else
				{
					ToggleFullscreen(); gbFullscreen = false;
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
		if(dwStyle & WS_OVERLAPPEDWINDOW)
		{
			mi = { sizeof(MONITORINFO) };
			if(GetWindowPlacement(ghwnd, &wpPrev) && 
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
	ZeroMemory((void *)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
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

	//d3dclearcolor(blue)
	gClearColor[0] = 0.0f;
	gClearColor[1] = 0.0f;
	gClearColor[2] = 1.0f;
	gClearColor[3] = 1.0f;

	//call resize for first time
	hr = resize(WIN_WIDTH, WIN_HEIGHT);
	if(FAILED(hr))
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

HRESULT resize(int width, int height)
{
	//code
	HRESULT hr = S_OK;

	//free any size dependant resources
	if(gpID3D11RenderTargetView)\
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
	ID3D11Texture2D * pID3DHTexture2D_BackBuffer;
	gpIDXGISwapChain->GetBuffer(0, 
		_uuidof(ID3D11Texture2D), 
		(LPVOID*)&pID3DHTexture2D_BackBuffer);

	//again get render target view from d3d11 device using above back buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3DHTexture2D_BackBuffer, 
		NULL, 
		&gpID3D11RenderTargetView);

	if(FAILED(hr))
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() Failed.\n ");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "ID3D11Device::CreateRenderTargetView() Succeeded. ");
		fprintf_s(gpFile, "With Width:%d and Height:%d.\n", width, height);
		fclose(gpFile);
	}
	pID3DHTexture2D_BackBuffer->Release();
	pID3DHTexture2D_BackBuffer = NULL;

	//set render target view as render target
		gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	//Set viewport
	D3D11_VIEWPORT d3dViewPort;
	d3dViewPort.TopLeftX = 0;
	d3dViewPort.TopLeftY = 0;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);
	return(hr);
}

void display(void)
{
	//code
	//clear render target view to a chosen color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, gClearColor);
	//switch between front & back buffers
	gpIDXGISwapChain->Present(0, 0);
}
void uninitialize(void)
{
	//code
	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	if(gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	if(gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if(gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	if(gpFile)
	{
		fopen_s(&gpFile, gszLogFileName, "a+");
		fprintf_s(gpFile, "uninitialize() Succeeded.\n");
		fprintf_s(gpFile, "Log File is Successfully Closed.\n ");
		fclose(gpFile);
	}
}