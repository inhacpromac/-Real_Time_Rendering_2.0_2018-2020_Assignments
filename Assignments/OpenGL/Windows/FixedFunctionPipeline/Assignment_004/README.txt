MessageBox - Messages Handling Program Using Win32SDK

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MyWindow:Assignment_002 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//WndProc
    // add following in switch
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

//End