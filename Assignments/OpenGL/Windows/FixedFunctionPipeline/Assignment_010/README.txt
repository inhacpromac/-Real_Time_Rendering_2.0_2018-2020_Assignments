WM_CHAR - handle f/f keypress for fullscreen 

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Fullscreen:Assignment_009 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add this Line
// headers
#include<stdio.h>

//Add thses Lines WndProc
switch (iMsg)
	{
	case WM_CHAR:
		switch (wParam)
		{
		case 'f':
		case 'F':
			MessageBox(hwnd, TEXT("'F' Key Is Pressed..."), TEXT("My Messages"), MB_OK);
			break;
		}
		break;
	.
	.
	.
	.
//End