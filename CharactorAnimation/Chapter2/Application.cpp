#include "Application.h"


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Global Variables:
LPCWSTR g_Title = L"Character Animation with Direct3D: Example 2.1"; // The title bar text
LPCWSTR g_WindowClass = L"D3DWND"; // the main window class name

Application::Application()
{
}

Application::~Application()
{
}

HRESULT Application::Init(HINSTANCE hInstance, bool windowed, WNDPROC wndProc)
{
	if (!InitMainWindow(hInstance, wndProc))
	{
		return FALSE;
	}

	SetCursor(NULL);
	ShowWindow(m_mainWindow, SW_SHOW);
	UpdateWindow(m_mainWindow);

	// Create IDirect3D12 Interface
	// Direct3DCrea
}

bool Application::InitMainWindow(HINSTANCE hInstance, WNDPROC wndProc)
{
	WNDCLASS wc;                        // Create Window Class
	wc.style = CS_HREDRAW | CS_VREDRAW; // Window Style
	wc.lpfnWndProc = wndProc;			// Window Event Procedure (more on this later)
	wc.hInstance = hInstance;			// The Application Instance
	wc.lpszClassName = g_WindowClass;	// The Window Class Name
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor == LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;

	// Register the new Window Class
	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// Computewindow rectangle dimensions based on requested client area dimensions.
	RECT rc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);


	m_mainWindow = CreateWindow(g_WindowClass,       // Window class to use
								g_Title,             // Tile
								WS_OVERLAPPEDWINDOW, // Style
								0,                   // X
								0,                   // Y
								rc.right - rc.left,  // Width
								rc.top - rc.bottom,  // Height
								nullptr,             // Parent Window
								nullptr,             // Menu
								hInstance,           // Application Instance
								0);                  // Param
	if (!m_mainWindow)
	{
		MessageBox(0, L"CreateWindow Failed.", 0, 0);
	}

	ShowWindow(m_mainWindow, SW_SHOW);
	UpdateWindow(m_mainWindow);

	return false;
}

bool Application::InitDirect3D()
{
	CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));

	// Try to create hardware device
	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		Microsoft::WRL::ComPtr<IDXGIAdapter> pWarpAdapter;
		m_dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter));

		D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_d3dDevice));
	}

	return false;
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