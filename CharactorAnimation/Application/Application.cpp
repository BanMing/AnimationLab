#include "Application.h"
#include <fstream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

//Global Variables
IDirect3DDevice9* g_pDevice = NULL;
ID3DXSprite* g_pSprite = NULL;
ID3DXFont* g_pFont = NULL;
ID3DXEffect* g_pEffect = NULL;
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

	m_mainWindow = CreateWindow(L"D3DWND", L"Window Tile", WS_EX_TOPMOST, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, 0);

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
	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_mainWindow, vp, &m_present, &g_pDevice)))
	{
		g_debug << "Failed to create IDirect3DDevice9 \n";
		return E_FAIL;
	}

	// Release IDirect3D9 interface (you don't need it anymore)
	d3d9->Release();

	//Load Application Specific resources here...
	D3DXCreateFont(g_pDevice, 20, 0, FW_BOLD, 1, false,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Arial", &g_pFont);

	//Create Sprite
	D3DXCreateSprite(g_pDevice, &g_pSprite);

	// Load Effect
	ID3DXBuffer* pErrorMsgs = NULL;
	HRESULT hRes = D3DXCreateEffectFromFile(g_pDevice, L"../resources/fx/lighting.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &g_pEffect, &pErrorMsgs);

	if (FAILED(hRes) && (pErrorMsgs != NULL))
	{
		MessageBox(NULL, (LPCWSTR)(char*)pErrorMsgs->GetBufferPointer(), L"Effect Error", MB_OK);
		return E_FAIL;
	}

	// Load Soldier Mesh
	m_soldier.Load(g_pDevice, L"../resources/meshes/soldier.x");
	m_deviceLost = false;

	return S_OK;
}

void Application::Update(float deltaTime)
{
	//Check for lost device
	HRESULT coop = g_pDevice->TestCooperativeLevel();

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

	// Camera Rotation 
	m_angle += deltaTime;
}

void Application::Render()
{
	if (m_deviceLost)
	{
		return;
	}
	D3DXMATRIX identity, shadow;
	D3DXMatrixIdentity(&identity);

	//Set ground plane + light position
	D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

	//Create the shadow matrix
	D3DXMatrixShadow(&shadow, &lightPos, &ground);

	// Calculate Transformation Matrices
	D3DXMATRIX view, proj, world;
	D3DXMatrixIdentity(&world);
	D3DXVECTOR3 angleVec3 = D3DXVECTOR3(cos(m_angle) * 2.0f, 2.0f, sin(m_angle) * 2.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&view, &angleVec3, &up, &up);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	//Set transformation matrices
	g_pDevice->SetTransform(D3DTS_WORLD, &world);
	g_pDevice->SetTransform(D3DTS_VIEW, &view);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &proj);
	D3DXMATRIX vp = view * proj;
	g_pEffect->SetMatrix("matVP", &vp);

	// Clear the viewport
	g_pDevice->Clear(
		0,					// Num rectangles to clear
		NULL,				// Rectangles to clear (NULL = whole screen)
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,	// Clear the render target
		0xffffffff,			// Color AARRGGBB (in this case White) 
		1.0f,				// Clear Z-buffer to 1.0f
		0);					 //Clear Stencil Buffer to 0

	// Begin the scene
	if (SUCCEEDED(g_pDevice->BeginScene()))
	{
		//Render Soldier
		{
			g_pEffect->SetMatrix("matW", &identity);
			g_pEffect->SetVector("lightPos", &lightPos);
			D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Lighting");
			g_pEffect->SetTechnique(hTech);
			g_pEffect->Begin(NULL, NULL);
			g_pEffect->BeginPass(0);

			m_soldier.Render();

			g_pEffect->EndPass();
			g_pEffect->End();
		}

		//Render Shadow
		{
			g_pEffect->SetMatrix("matW", &shadow);
			D3DXHANDLE hTech = g_pEffect->GetTechniqueByName("Shadow");
			g_pEffect->SetTechnique(hTech);
			g_pEffect->Begin(NULL, NULL);
			g_pEffect->BeginPass(0);

			m_soldier.Render();

			g_pEffect->EndPass();
			g_pEffect->End();
		}

		// End the scene.
		g_pDevice->EndScene();

		// Present the result (full screen)
		g_pDevice->Present(0, 0, 0, 0);
	}
}

void Application::Cleanup()
{
	//Release all resources here...
	if (g_pSprite != NULL)	g_pSprite->Release();
	if (g_pFont != NULL)		g_pFont->Release();
	if (g_pDevice != NULL)	g_pDevice->Release();
	if (g_pEffect != NULL)	g_pEffect->Release();
}

void Application::Quit()
{
	DestroyWindow(m_mainWindow);
	PostQuitMessage(0);
}

void Application::DeviceLost()
{
	g_pFont->OnLostDevice();
	g_pSprite->OnLostDevice();
	g_pEffect->OnLostDevice();
	m_deviceLost = true;
}

void Application::DeviceGained()
{
	g_pDevice->Reset(&m_present);
	g_pFont->OnResetDevice();
	g_pSprite->OnResetDevice();
	g_pEffect->OnResetDevice();
	m_deviceLost = false;
}
