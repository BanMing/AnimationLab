#pragma once
#include "Application.h"
#include "../Skinning/SkinnedMesh.h"
#include <vector>

class Example4_3 :public Application
{
public:
	Example4_3();
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
	void RandomizeAnimations();

private:
	SkinnedMesh m_drone;
	float m_deltaTime;
	ID3DXEffect* m_pSkinningEffect;
	std::vector<ID3DXAnimationController*> m_animControllers;
	std::vector<D3DXMATRIX> m_position;
};

