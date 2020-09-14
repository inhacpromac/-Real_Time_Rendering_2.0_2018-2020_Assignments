getDC - Hello World using getDC

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MyWindow:Assignment_002 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add these Lines in WndProc
///////////////////////////////////////////////////////////////////////////////////////////////////
	// Variable Declaration
	HDC hdc;
	RECT rc;
	TCHAR str[] = TEXT("Hello World!!!");

	// code
	switch (iMsg) 
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rc);
		hdc = GetDC(hwnd);
		SetBkColor(hdc, RGB(0, 0, 0));
		SetTextColor(hdc, RGB(0, 255, 0));
		DrawText(hdc, str, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
		ReleaseDC(hwnd, hdc);
		break;
///////////////////////////////////////////////////////////////////////////////////////////////////
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

//End