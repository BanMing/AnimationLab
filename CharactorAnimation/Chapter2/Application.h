#pragma once

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>

// Link necessary d3d12 libraries.
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace std;

class Application
{
public:
	Application();
	~Application();
	HRESULT Init(HINSTANCE hInstance, bool windowed, WNDPROC wndProc);
	bool InitMainWindow(HINSTANCE hInstance, WNDPROC wndProc);
	bool InitDirect3D();
	void Update(float deltaTime);
	void Render();
	void Cleanup();
	void Quit();

	void DeviceLost();
	void DeviceGained();

private:
	HWND m_mainWindow;
	
	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
};
