#pragma once
#include "Application.h"
#include "../Skinning/SkinnedMesh.h"

class Example5_1 :public Application
{
public:
	Example5_1();

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
	void InitAnimationTrack();
	void DrawTrackStatus();
private:
	SkinnedMesh m_drone;
	ID3DXAnimationController* m_pAnimController;
	ID3DXEffect* m_pSkinningEffect;
};

