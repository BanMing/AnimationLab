#include "Application.h"
#include <fstream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

std::ofstream g_debug("debug.txt");

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// User specified events
	switch (msg)
	{
	case WM_CREATE:
		// Do some window initialization here
		break;
	case WM_DESTROY:
		// Do some window cleanup here
		PostQuitMessage(0);
		break;
	}

	// Default events
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Application::Application()
{
	m_appName = L"Character Animation";
}

Application::~Application()
{
}

HRESULT Application::Init(HINSTANCE hInstance, bool windowed)
{
	// Create Window Class
	WNDCLASS wc;
	memset(&wc, 0, sizeof(WNDCLASS));

	// Windows Style
	wc.style = CS_HREDRAW | CS_VREDRAW;

	// Window Event Procedure (more on this later)
	wc.lpfnWndProc = (WNDPROC)WndProc;

	// The Application Instance
	wc.hInstance = hInstance;

	// The Window Class Name
	wc.lpszClassName = L"D3DWND";

	//... Finally Register the new Window Class
	RegisterClass(&wc);

	m_mainWindow = CreateWindow(L"D3DWND", m_appName, WS_EX_TOPMOST, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, 0);

	SetCursor(NULL);

	// Display the new window
	ShowWindow(m_mainWindow, SW_SHOW);

	// Update it 
	UpdateWindow(m_mainWindow);

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

	if (d3d9 == NULL)
	{
		printf("Could not create the Direct3D interface, Exit...\n");
		return E_FAIL;
	}


	//Check that the Device supports what we need from it
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	//Check vertex & pixelshader versions
	if (caps.VertexShaderVersion < D3DVS_VERSION(2, 0) || caps.PixelShaderVersion < D3DPS_VERSION(2, 0))
	{
		g_debug << "Warning - Your graphic card does not support vertex and pixelshaders version 2.0 \n";
	}

	//Set D3DPRESENT_PARAMETERS
	m_present.BackBufferWidth = WINDOW_WIDTH;
	m_present.BackBufferHeight = WINDOW_HEIGHT;
	m_present.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_present.BackBufferCount = 2;
	m_present.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_present.MultiSampleQuality = 0;
	m_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_present.hDeviceWindow = m_mainWindow;
	m_present.Windowed = windowed;
	m_present.EnableAutoDepthStencil = true;
	m_present.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_present.Flags = 0;
	m_present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	//Hardware Vertex Processing
	int vp = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//Create the IDirect3DDevice9
	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_mainWindow, vp, &m_present, &m_pDevice)))
	{
		g_debug << "Failed to create IDirect3DDevice9 \n";
		return E_FAIL;
	}

	// Release IDirect3D9 interface (you don't need it anymore)
	d3d9->Release();

	//Load Application Specific resources here...
	D3DXCreateFont(m_pDevice, 20, 0, FW_BOLD, 1, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Arial", &m_pFont);

	//Create Sprite
	D3DXCreateSprite(m_pDevice, &m_pSprite);

	m_deviceLost = false;
	printf("$$$$$$$$\n");
	return OnInit();
}

void Application::Update(float deltaTime)
{
	//Check for lost device
	HRESULT coop = m_pDevice->TestCooperativeLevel();

	if (coop != D3D_OK)
	{
		if (coop == D3DERR_DEVICELOST)
		{
			if (m_deviceLost == false)
				DeviceLost();
		}
		else if (coop == D3DERR_DEVICENOTRESET)
		{
			if (m_deviceLost == true)
				DeviceGained();
		}

		Sleep(100);
		return;
	}

	OnUpdate(deltaTime);
}

void Application::Render()
{
	if (m_deviceLost)
	{
		return;
	}
	OnRender();
}

void Application::Cleanup()
{
	//Release all resources here...
	if (m_pSprite != NULL)	m_pSprite->Release();
	if (m_pFont != NULL)		m_pFont->Release();
	if (m_pDevice != NULL)	m_pDevice->Release();
}

void Application::Quit()
{
	DestroyWindow(m_mainWindow);
	PostQuitMessage(0);
}

void Application::DeviceLost()
{
	m_pFont->OnLostDevice();
	m_pSprite->OnLostDevice();
	m_deviceLost = true;
}

void Application::DeviceGained()
{
	m_pDevice->Reset(&m_present);
	m_pFont->OnResetDevice();
	m_pSprite->OnResetDevice();
	m_deviceLost = false;
}
