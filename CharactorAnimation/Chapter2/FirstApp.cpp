/******************************************************************
** InitDirect3DApp.cpp
** @Author       : BanMing
** @Date         : 8/4/2021 11:28:09 PM
** @Description  :
*******************************************************************/

#include "Application.h"

class FirstApp : public Application
{
public:
	FirstApp();
	~FirstApp();


private:
	virtual void OnResize() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;
};

FirstApp::FirstApp()
{
}

FirstApp::~FirstApp()
{
}

void FirstApp::OnResize()
{
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
	//m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition());

	// Set the viewport and cull rect. This needs to be reset.
	// whenever the command list is reset
	m_commandList->RSSetScissorRects(1,&m_win)
}
