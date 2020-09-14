CenterWindow - Open the window in the center of the screen

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MyWindow:Assignment_002 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add thses Lines WINMAIN

int nWidth = 800, nHeight = 600;

	// create window
	hwnd = CreateWindow(szAppName,
		TEXT("My Window - Rohan V. Waghmode"),
		WS_OVERLAPPEDWINDOW,
		((GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2),
		((GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2),
		nWidth,
		nHeight,
		NULL,
		NULL,
		hInstance,
		NULL);

//End