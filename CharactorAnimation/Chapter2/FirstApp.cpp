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
	m_commandQueue->ExecuteCommandLists(_countof(cmdsLists),cmdsLists);

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
}

void FirstApp::BuildConstantBuffers()
{
}

void FirstApp::BuildRootSignature()
{
}

void FirstApp::BuildShadersAndInputLayout()
{
}

void FirstApp::BuildBoxGeometry()
{
}

void FirstApp::BuildPSO()
{
}

#pragma endregion
