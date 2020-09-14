// Assignment_004
// MessageBox - Messages Handling Program Using Win32SDK

// headers
#include<windows.h>

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// WinMainQ
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
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL);

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

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// code
	switch (iMsg)
	{
	case WM_CREATE:
		MessageBox(hwnd, TEXT("Hello!! This is WM_CREATE. Your Window is created."), TEXT("My Message"), MB_OK);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			MessageBox(hwnd, TEXT("'ESC' Key Is Pressed..."), TEXT("My Messages"), MB_OK);
			DestroyWindow(hwnd);
			break;

		case 0x46:
			MessageBox(hwnd, TEXT("'F' Key Is Pressed..."), TEXT("My Messages"), MB_OK);
			break;
		}
		break;

	case WM_LBUTTONDOWN:
		MessageBox(hwnd, TEXT("Left Mouse Button Clicked..."), TEXT("My Messages"), MB_OK);
		break;

	case WM_RBUTTONDOWN:
		MessageBox(hwnd, TEXT("Right Mouse Button Clicked..."), TEXT("My Messages"), MB_OK);
		break;

	case WM_DESTROY:
		MessageBox(hwnd, TEXT("This is WM_DESTORY. Bye!! See you soon."), TEXT("My Message"), MB_OK);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}