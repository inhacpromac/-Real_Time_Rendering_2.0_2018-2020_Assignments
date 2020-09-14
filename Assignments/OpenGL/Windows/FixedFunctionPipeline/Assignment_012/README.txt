DoubleBuffer

///////////////////////////////////////////////////////////////////////////////////////////////////
//Copied SingleBuffer:Assignment_011 Changes Made to That As Follows
///////////////////////////////////////////////////////////////////////////////////////////////////

//Add thses Lines WINMAIN

// Gameloop
	while (bDone == false)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActiveWindow == true)
			{
				// Here call update
			}
			display();//////////////////////////////////////////////this line
		}


	}
	
//remove MessageBox from WNDPROC

//Add this line instead of glFlush();
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	SwapBuffers(ghdc);//////////////////////////this line
}

//Add this line under uninitialize
if (gbFullScreen == true)
	{
		SetWindowLong(ghwnd, GWL_STYLE, dwstyle | WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER);

		ShowCursor(TRUE);
	}

//End