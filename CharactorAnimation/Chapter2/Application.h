#pragma once

//#include "d3dx12.h"
#include <windows.h>

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;


class Application
{
public:
	Application();
	~Application();
	HRESULT Init(HINSTANCE hInstance, bool windowed, WNDPROC wndProc);
	void Update(float deltaTime);
	void Render();
	void Cleanup();
	void Quit();

	void DeviceLost();
	void DeviceGained();

private:
	HWND m_mainWindow;
	bool m_deviceLost;
	//D3DPRESENT_PARAMETERS m_present;
};
