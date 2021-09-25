/******************************************************************
** FirstApp.h
** @Author       : BanMing
** @Date         : 8/10/2021 10:35:42 PM
** @Description  :
*******************************************************************/

#pragma once

#include "Application.h"
#include "UploadBuffer.h"
#include "MathHelper.h"
#include <vector>


struct ObjectConstants
{
	DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class FirstApp : public Application
{
public:
	FirstApp();
	~FirstApp();

	virtual bool Initialize(HINSTANCE hInstance, bool windowed, WNDPROC wndProc) override;
	virtual void OnResize() override;
	virtual void Update(float deltaTime) override;
	virtual void Render() override;

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildBoxGeometry();
	void BuildPSO();

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> m_objectCB = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> m_vsByteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> m_psByteCode = nullptr;


	std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
};