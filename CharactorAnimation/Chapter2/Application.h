#pragma once

#include "D3dx12.h"

#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <cassert>
#include <string>

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

	bool Initialize(HINSTANCE hInstance, bool windowed, WNDPROC wndProc);
	bool InitMainWindow(HINSTANCE hInstance, WNDPROC wndProc);

	void Cleanup();
	void Quit();

public:
	virtual void OnResize();
	virtual void Update(float deltaTime);
	virtual void Render();

protected:
	ID3D12Resource* CurrentBackBuffer() const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	void FlushCommandQueue();

private:
	void ResetSwapChain();
	void SwapChainRender();

	void CreateDepthBufferView();
	D3D12_RESOURCE_DESC CreateDepthStencilDesc();

	void CreateViewportScissor();

protected:
	static const int SwapChainBufferCount = 2;

	HWND m_mainWindow;

	// Set true to use 4X MSAA. The default is false
	bool m_4xMsaaState = false; // 4X MSAA enabled
	UINT m_4xMsaaQuality = 0;   // quality level of 4X MSAA

	Microsoft::WRL::ComPtr<ID3D12Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;

	Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
	UINT64 m_currentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_directCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

	int m_currBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> m_swapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> m_depthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

	UINT m_rtvDescriptorSize = 0;
	UINT m_dsvDescriptorSize = 0;
	UINT m_cbvSrvUavDescriptorSize = 0;

	D3D12_VIEWPORT m_screenViewport;
	D3D12_RECT m_scissorRect;

	// Derived class should set these in derived constructor to customize starting values.
	std::wstring m_appTile = L"Charactor Animation";
	std::wstring m_windowClassName = L"MainWnd";
	D3D_DRIVER_TYPE m_d3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT m_depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int m_windowWidth = 800;
	int m_windowHeight = 600;
};
