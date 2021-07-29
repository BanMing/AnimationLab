#include "Application.h"

Application::Application()
{
}

Application::~Application()
{
}

#pragma region Initialize

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
	WNDCLASS wc;							// Create Window Class
	wc.style = CS_HREDRAW | CS_VREDRAW;		// Window Style
	wc.lpfnWndProc = wndProc;				// Window Event Procedure (more on this later)
	wc.hInstance = hInstance;				// The Application Instance
	wc.lpszClassName = m_windowClassName.c_str();	// The Window Class Name
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
	RECT rc = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);


	m_mainWindow = CreateWindow(m_windowClassName.c_str(),  // Window class to use
								m_appTile.c_str(),          // Tile
								WS_OVERLAPPEDWINDOW,		// Style
								0,							// X
								0,							// Y
								rc.right - rc.left,			// Width
								rc.top - rc.bottom,			// Height
								nullptr,					// Parent Window
								nullptr,					// Menu
								hInstance,					// Application Instance
								0);							// Param
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

	m_d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

	// Set render quality
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	m_d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
									 &msQualityLevels,
									 sizeof(msQualityLevels));

	m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

	return true;
}

void Application::CreateCommandObjects()
{
	// Create command queue
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	m_d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));

	// Create command allocator
	m_d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_directCmdListAlloc.GetAddressOf()));

	// Start off in a closed state. This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	m_commandList->Close();
}

void Application::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	m_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = m_windowWidth;
	sd.BufferDesc.Height = m_windowHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m_4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m_4xMsaaState ? (m_4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = m_mainWindow;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to preform flush.
	m_dxgiFactory->CreateSwapChain(m_commandQueue.Get(), &sd, m_swapChain.GetAddressOf());
}

void Application::CreateRtvAndDsvDescriptorHeaps()
{
	m_rtvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_dsvDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	m_cbvSrvUavDescriptorSize = m_d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	m_d3dDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_rtvHeap.GetAddressOf()));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	m_d3dDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_dsvHeap.GetAddressOf()));

}

#pragma endregion


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