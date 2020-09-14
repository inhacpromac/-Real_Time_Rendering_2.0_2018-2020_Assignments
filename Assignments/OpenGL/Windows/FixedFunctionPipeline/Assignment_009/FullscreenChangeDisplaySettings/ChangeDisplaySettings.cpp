// Assignment_009
// FullScreen -  ChangeDisplaySettings()

// headers
#include<windows.h>

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool bFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND gHwnd = NULL;

// WinMain
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpsvCmdLine, int iCmdShow)
{
	//variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("MyApp");

	// code
	// initialization of WNDCLASSEX
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	// register above class
	RegisterClassEx(&wndclass);

	// create window
	hwnd = CreateWindow(szAppName,
		TEXT("My Window - Rohan V. Waghmode"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);

	gHwnd = hwnd;

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	// message loop
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return((int)msg.wParam);
}

void ToggleFullScreen(void)
{
	//Code
	DEVMODE devMode = { sizeof(DEVMODE) };

	if (bFullScreen)
	{
		SetWindowLong(gHwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
		SetWindowPos(gHwnd, HWND_TOP, 0, 0, 800, 600, SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
		bFullScreen = 0;
	}
	else
	{
		dwStyle = GetWindowLong(gHwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			EnumDisplaySettings(NULL, 0, &devMode);
			ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
			SetWindowLong(gHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
			ShowCursor(FALSE);
		}
		bFullScreen = 1;
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// code
	switch (iMsg)
	{
	//Function declaration 
	void ToggleFullScreen(void);

	// handle following message
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 0x46:
			ToggleFullScreen();
			break;
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}