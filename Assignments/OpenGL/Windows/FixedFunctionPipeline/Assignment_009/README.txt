FullscreenChangeDisplaySettings - Fullscreen using ChangeDisplaySettings

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MyWindow:Assignment_002 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add thses Lines under
// global function declarations
bool bFullScreen = false;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
HWND gHwnd = NULL;

//Add thses Lines under WINMAIN
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


//Add thses Lines

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

//End