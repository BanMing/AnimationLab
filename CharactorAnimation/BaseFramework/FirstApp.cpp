/******************************************************************
** FirstApp.cpp
** @Author       : BanMing
** @Date         : 8/4/2021 11:28:09 PM
** @Description  :
*******************************************************************/

#include "FirstApp.h"

FirstApp::FirstApp() :Application()
{
}

FirstApp::~FirstApp()
{
}

bool FirstApp::Initialize(HINSTANCE hInstance, bool windowed, WNDPROC wndProc)
{
	if (!Application::Initialize(hInstance, windowed, wndProc))
	{
		return false;
	}

	// Reset the command list to prep for initialization commands.
	m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr);

	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();

	// Execute the initialization commands.
	m_commandList->Close();
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until initialization is complete
	FlushCommandQueue();

	return true;
}

void FirstApp::OnResize()
{
	Application::OnResize();
}

void FirstApp::Update(float deltaTime)
{
}

void FirstApp::Render()
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished
	// execution on the GPU
	m_directCmdListAlloc->Reset();

	// A command list can be reset after it has been added to the 
	// command queue via ExecuteCommandList . Reusing the command list reuses memory
	m_commandList->Reset(m_directCmdListAlloc.Get(), nullptr);

	// Indicate a state transition on the resource usage.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
																			D3D12_RESOURCE_STATE_PRESENT,
																			D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Set the viewport and cull rect. This needs to be reset.
	// whenever the command list is reset
	m_commandList->RSSetViewports(1, &m_screenViewport);
	m_commandList->RSSetScissorRects(1, &m_scissorRect);

	// Clear the back buffer and depth buffer.
	m_commandList->ClearRenderTargetView(CurrentBackBufferView(),
										 DirectX::Colors::BurlyWood,
										 0,
										 nullptr);

	m_commandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	m_commandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	// Indicate a state transition on the resource usage.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
																			D3D12_RESOURCE_STATE_RENDER_TARGET,
																			D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands
	m_commandList->Close();

	// Add the command list to the queue for excution
	ID3D12CommandList* cmdsLists[] = { m_commandList.Get() };
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	m_swapChain->Present(0, 0);
	m_currBackBuffer = (m_currBackBuffer + 1) % SwapChainBufferCount;

	// Wait unitl frame commands are complete.
	// This waiting is inefficient and is done for simplicity .
	// Later we will show how to organize our readering code so we do not have to wait per frame
	FlushCommandQueue();
}


#pragma region Build Reader Config

void FirstApp::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	m_d3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap));
}

void FirstApp::BuildConstantBuffers()
{
	m_objectCB = std::make_unique<UploadBuffer<ObjectConstants>>(m_d3dDevice.Get(), 1, true);

	UINT objCBByteSize = CalcConstantBufferBytesSize(sizeof(ObjectConstants));

	// the start of buffer address(the 0th index of the constant buffer address)
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_objectCB->Resource()->GetGPUVirtualAddress();

	// Offset to the ith object constant buffer in the buffer.
	int boxCBufIndex = 0;
	cbAddress += boxCBufIndex * objCBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CalcConstantBufferBytesSize(sizeof(ObjectConstants));

	m_d3dDevice->CreateConstantBufferView(&cbvDesc, m_cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void FirstApp::BuildRootSignature()
{
	// Shader programs typically require resources as input (constant buffers,
	// textures,samples). The root signature defines the resources the shader
	// programs expect. If we think of the shader programs as a function, and
	// the input resources as function parameters, then the root signature can be 
	// thought of as defining the function signature.

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[1];

	// Create a single descriptor table of CBVs.
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr,
											D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// Create a root signature with a single solt which points to a descriptor range
	// consisting of a single constant buffer
	Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
											 serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}

	m_d3dDevice->CreateRootSignature(0,
									 serializedRootSig->GetBufferPointer(),
									 serializedRootSig->GetBufferSize(),
									 IID_PPV_ARGS(&m_rootSignature));
}

void FirstApp::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	m_vsByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	m_psByteCode = d3dUtil::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	m_inputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

}

void FirstApp::BuildBoxGeometry()
{
}

void FirstApp::BuildPSO()
{
}

#pragma endregion
