#include "Application.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Global Variables:
LPCWSTR g_Title = LPCWSTR("Character Animation with Direct3D: Example 2.1");                  // The title bar text
LPCWSTR g_WindowClass = LPCWSTR("D3DWND");           // the main window class name

Application::Application()
{

}

Application::~Application()
{

}

HRESULT Application::Init(HINSTANCE hInstance, bool windowed, WNDPROC wndProc)
{
	// Create Window Class
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));

	// Window Style
	wc.style = CS_HREDRAW | CS_VREDRAW;

	// Window Event Procedure (more on this later)
	wc.lpfnWndProc = wndProc;

	// The Application Instance
	wc.hInstance = hInstance;

	// The Window Class Name
	wc.lpszClassName = g_WindowClass;

	RECT rc = { 0,0,WINDOW_WIDTH,WINDOW_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	// Register the new Window Class
	RegisterClass(&wc);

	m_mainWindow = CreateWindow(
		g_WindowClass,		 // Window class to use
		g_Title,             // Tile
		WS_OVERLAPPEDWINDOW, // Style
		0,					 // X
		0,					 // Y
		rc.right - rc.left,  // Width
		rc.top - rc.bottom,  // Height
		nullptr,			 // Parent Window
		nullptr,			 // Menu
		hInstance,			 // Application Instance
		0);					 // Param

	SetCursor(NULL);
	ShowWindow(m_mainWindow, SW_SHOW);
	UpdateWindow(m_mainWindow);

	// Create IDirect3D12 Interface
	//Direct3DCrea
}

void Application::Update(float deltaTime)
{

}

void Application::Render()
{

}

void Application::Cleanup()
{

}

void Application::Quit()
{
	DestroyWindow(m_mainWindow);
}

void Application::DeviceLost()
{

}

void Application::DeviceGained()
{

}