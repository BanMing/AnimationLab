#pragma once
#include <windows.h>
#include <d3dx9.h> 
#include <fstream>
#include "../Resources/Mesh.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

class Application
{
public:
	Application();
	~Application();
	HRESULT Init(HINSTANCE hInstance, bool windowed);
	void Update(float deltaTime);
	void Render();
	void Cleanup();
	void Quit();

	void DeviceLost();
	void DeviceGained();
protected:
	virtual HRESULT OnInit() = 0;
	virtual void OnUpdate(float deltaTime) = 0;
	virtual void OnRender() = 0;
	virtual void OnCleanup() = 0;
	virtual void OnQuit() = 0;

	virtual void OnDeviceLost() = 0;
	virtual void OnDeviceGained() = 0;
protected:
	LPCWSTR m_appName;

	IDirect3DDevice9* m_pDevice;
	ID3DXSprite* m_pSprite = NULL;
	ID3DXFont* m_pFont = NULL;

	bool m_deviceLost;
	HWND m_mainWindow;
	D3DPRESENT_PARAMETERS m_present;
};
