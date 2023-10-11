#pragma once
#include "Application.h"
#include "../Skinning/SkinnedMesh.h"

class Example3_1 :public Application
{
public:
	Example3_1();
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
	SkinnedMesh m_drone;
	float m_time;
};

