#pragma once
#include "Application.h"
class Example2_1 :public Application
{
public:
	Example2_1();

protected:
	// Inherited via Application
	HRESULT OnInit() override;
	void OnUpdate(float deltaTime) override;
	void OnRender() override;
	void OnCleanup() override;
	void OnQuit() override;
	void OnDeviceLost() override;
	void OnDeviceGained() override;

private:
	Mesh m_soldier;
	float m_angle;
	ID3DXEffect* m_pEffect = NULL;
};

