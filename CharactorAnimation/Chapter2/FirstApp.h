/******************************************************************
** FirstApp.h
** @Author       : BanMing
** @Date         : 8/10/2021 10:35:42 PM
** @Description  :
*******************************************************************/

#pragma once

#include "Application.h"

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
};