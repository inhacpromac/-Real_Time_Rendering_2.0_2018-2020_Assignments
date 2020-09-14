MessageBox - Messages Handling Program Using Win32SDK

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied MyWindow:Assignment_002 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//WndProc
    // add following 
		//Function declaration 
		void ToggleFullScreen(void);
		
		// handle following message
		case WM_KEYDOWN:
			switch (wParam)
			{
				case 0x46: // 'f'
					ToggleFullScreen();
					break;
			}
			break;


// Add ToggleFullscreen() function
	void ToggleFullScreen(void)
	{
		//Code
		MONITORINFO mi;

		if (bFullScreen == false)
		{
			dwStyle = GetWindowLong(gHwnd, GWL_STYLE);

			if (dwStyle & WS_OVERLAPPEDWINDOW)
			{
				mi = {sizeof(MONITORINFO)};

				if (GetWindowPlacement(gHwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(gHwnd, MONITORINFOF_PRIMARY), &mi))
				{
					SetWindowLong(gHwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

					SetWindowPos(gHwnd,
								HWND_TOP,
								mi.rcMonitor.left,
								mi.rcMonitor.top,
								mi.rcMonitor.right - mi.rcMonitor.left,
								mi.rcMonitor.bottom - mi.rcMonitor.top,
								SWP_NOZORDER | SWP_FRAMECHANGED);
				}
			}
			
			ShowCursor(FALSE);
			
			bFullScreen = true;
		}
		else
		{
			SetWindowLong(gHwnd,
						GWL_STYLE,
						dwStyle | WS_OVERLAPPEDWINDOW);
			
			SetWindowPlacement(gHwnd, &wpPrev);
			
			SetWindowPos(gHwnd,
						HWND_TOP,
						0,
						0,
						0,
						0,
						SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);
			
			ShowCursor(TRUE);
			
			bFullScreen = false;
		}
	}

//End