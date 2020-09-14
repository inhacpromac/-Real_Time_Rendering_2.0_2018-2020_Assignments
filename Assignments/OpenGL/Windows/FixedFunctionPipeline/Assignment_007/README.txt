Paintstruct - Hello World using Piantstruct in BeginPaint under WM_PAINT

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MyWindow:Assignment_002 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add thses Lines WndProc

	// Variable Declaration
	HDC hdc;
	RECT rc;
	TCHAR str[] = TEXT("Hello World!!!");
	PAINTSTRUCT ps;

	// code
	switch (iMsg)
	{
	case WM_PAINT:
		GetClientRect(hwnd, &rc);
		hdc = BeginPaint(hwnd, &ps);
		SetBkColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 255, 0));
		DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		EndPaint(hwnd, &ps);
		break;
	.
	.
	.


//End