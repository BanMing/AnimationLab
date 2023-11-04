#pragma once
#include "Application.h"
class Example4_1 :public Application
{
public:
	Example4_1();
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
	void DrawLine();
private:
	ID3DXKeyframedAnimationSet* m_pAnimSet;
	float m_time;
	ID3DXLine* m_pLine;
};

