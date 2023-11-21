#pragma once
#include "Application.h"
#include "../Skinning/SkinnedMesh.h"

class Example5_2 :public Application
{
public:
	float m_show;
	Example5_2();

protected:
	// Inherited via Application
	HRESULT OnInit() override;
	void OnUpdate(float deltaTime) override;
	void OnRender() override;
	void OnCleanup() override;
	void OnQuit() override;
	void OnDeviceLost() override;
	void OnDeviceGained() override;

protected:
	class CallbackHandler : public ID3DXAnimationCallbackHandler
	{
	public:
		HRESULT CALLBACK HandleCallback(THIS_ UINT Track, LPVOID pCallbackData)
		{
			Example5_2* example = (Example5_2*)pCallbackData;
			example->m_show = 0.1f;
			return D3D_OK;
		}
	};
	void SetupCallback();

private:
	SkinnedMesh m_drone;
	ID3DXAnimationController* m_pAnimController;
	ID3DXEffect* m_pSkinningEffect;
	CallbackHandler m_animationCallback;
};

