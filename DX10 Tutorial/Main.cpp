//Platform includes
#include <Windows.h>

//Local includes
#include "resource.h"
#include "DXRenderer.h"

//Define the string to appear in the top left corner of the window
#define WINDOW_TITLE "DirectX 10 Tutorial"

//The name of the window class is the window title plus " Class"
#define WINDOW_CLASS WINDOW_TITLE " Class"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

//Global variables
DXRenderer *g_pDXRenderer(NULL);

//The function that will handle any messages sent to the window
LRESULT CALLBACK WndProc(HWND hwnd, UINT m, WPARAM wp, LPARAM lp)
{    
	//Check the message code
	switch(m)
	{    
		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0);
		   break;
		case WM_KEYDOWN:
			switch(wp)
			{ 
				case VK_ESCAPE:
					PostQuitMessage(0);
				  break;
				default:
				  break;
			}
			break;
		default:
		  break;
	}

	//Pass remaining messages to default handler.
	return (DefWindowProc(hwnd, m, wp, lp));
}

//This program's entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prev, LPSTR cmd, int show)
{	   
	//Struct to describe the main window.
	WNDCLASSEX windowClass;

	//Empty the windowClass struct
	memset(&windowClass, 0, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);		//Store the size of the struct
	windowClass.style = CS_HREDRAW | CS_VREDRAW;	//Redraw the window on horizontal or vertical size changes
	windowClass.lpfnWndProc = WndProc;				//The message handling function to be used by the window
	windowClass.hInstance = hInstance;				//Handle to the window's instance
	windowClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));	//Use the custom icon that we added to the project
	windowClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));//Use the custom icon that we added to the project
	windowClass.hCursor = LoadCursor(NULL, IDC_CROSS);//Use the standard "Windows arrow" cursor in this window
	windowClass.hbrBackground = (HBRUSH) COLOR_ACTIVEBORDER;//Use the "active border" colour for the window background
	windowClass.lpszClassName = WINDOW_CLASS;		//Assign a name to this window class

	//Attempt to register this window class with Windows
	if(!RegisterClassEx(&windowClass))
	{
		//Exit the program
		return -1;
	}

	//Create an, initially invisible, window
	HWND hwnd(CreateWindowEx(NULL, 
							 WINDOW_CLASS, 
							 WINDOW_TITLE,
							 WS_OVERLAPPEDWINDOW, 
							 100, 100,
							 WINDOW_WIDTH, WINDOW_HEIGHT,
							 0, 
							 0, 
							 hInstance, 
							 NULL));
   
	//If now window was created
	if(!hwnd)
	{
		//Exit the program
		return -1;
	}
  
	//Construct a new DXRenderer object
	g_pDXRenderer = new DXRenderer();

	//Stop the window from being invisible
	ShowWindow(hwnd, SW_SHOW);
  
	//Create a structure that will contain each message sent to the window
	MSG msg;
  
	//Get the internal dimensions of the window
	RECT windowRect;
	GetClientRect(hwnd, &windowRect);

	//If initialising the DXRenderer succeeds
	if(g_pDXRenderer->Initialise(hwnd, windowRect.right, windowRect.bottom) == 0)
	{
		//This is the message loop.
		while(true)
		{
			//Take a message from the message queue
			if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				//If a quit message then break;
				if(WM_QUIT == msg.message) 
				{
					break;
				}

				//Take a virtual-key message and change it to a MSG struct
				TranslateMessage(&msg);

				//Send this translated message to the message handling function
				DispatchMessage(&msg);
			}
			else
			{
				//Run the DirectX draw commands
				g_pDXRenderer->Render();
			}
		}		
	}

	//Deconstruct the DXRenderer
	delete g_pDXRenderer; 
  
	//Now that you're done with the window, unregister it!
	UnregisterClass(WINDOW_CLASS, windowClass.hInstance);

	//Exit the program with the appropriate return command
	return (int) msg.wParam;
}