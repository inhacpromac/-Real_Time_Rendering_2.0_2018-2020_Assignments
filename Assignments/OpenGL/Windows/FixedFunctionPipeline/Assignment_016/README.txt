glViewport to Display triangle in different parts of window

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied Perpective:Assignment_015 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add these lines under WNDPROC as case
case WM_KEYDOWN:
		switch (wParam)
		{
		case '1':
		case VK_NUMPAD1: //For 1 key
			glViewport(0, 0, w / 2, h / 2);
			break;
		case '2':
		case VK_NUMPAD2: //For 2 key
			glViewport(0, 0, w, h / 2);
			break;
		case '3':
		case VK_NUMPAD3: //For 3 key
			glViewport(w / 2, 0, w / 2, h / 2);
			break;
		case '4':
		case VK_NUMPAD4: //For 4 key
			glViewport(0, 0, w / 2, h);
			break;
		case '5':
		case VK_NUMPAD5: //For 5 key
			glViewport(0, 0, w, h);
			break;
		case '6':
		case VK_NUMPAD6: //For 6 key
			glViewport(w / 2, 0, w / 2, h);
			break;
		case '7':
		case VK_NUMPAD7: //For 7 key
			glViewport(0, h / 2, w / 2, h / 2);
			break;
		case '8':
		case VK_NUMPAD8: //For 8 key
			glViewport(0, h / 2, w, h / 2);
			break;
		case '9':
		case VK_NUMPAD9: //For 9 key
			glViewport(w / 2, h / 2, w / 2, h / 2);
			break;
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		case 0x46:
			ToggleFullScreen();
			break;
		}
		break;
		
//remove viewport call from resize();



//End