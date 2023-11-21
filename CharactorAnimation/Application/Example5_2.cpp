#include "Example5_2.h"
#include <iostream>
static D3DXVECTOR3 eye{ 0.0f, 1.5f, -3.0f };
static D3DXVECTOR3 at{ 0.0f, 1.0f, 0.0f };
static D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };

//Set ground plane + light position
static D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
static D3DXVECTOR4 lightPos(-50.0f, 75.0f, -150.0f, 0.0f);

Example5_2::Example5_2()
{
	m_appName = L"Example 5-2";
	m_pAnimController = NULL;
}

HRESULT Example5_2::OnInit()
{
	// Load Effect
	ID3DXBuffer* pErrorMsgs = NULL;
	HRESULT hRes = D3DXCreateEffectFromFile(m_pDevice, L"../resources/fx/skinning.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pSkinningEffect, &pErrorMsgs);

	if (FAILED(hRes) && (pErrorMsgs != NULL))
	{
		MessageBox(NULL, (LPCWSTR)pErrorMsgs->GetBufferPointer(), L"Effect Error", MB_OK);
		return E_FAIL;
	}

	// Load Model
	m_drone.Load(m_pDevice, L"../resources/meshes/soldier_5_1.x", SkinningType::GPU);
	
	SetupCallback();

	return S_OK;
}

void Example5_2::OnUpdate(float deltaTime)
{
	D3DXMATRIX world;
	D3DXMatrixIdentity(&world);
	m_drone.Update(world);
	m_pAnimController->AdvanceTime(deltaTime, &m_animationCallback);
	m_show -= deltaTime;
}

void Example5_2::OnRender()
{
	D3DXMATRIX identity, shadow, world, view, proj;
	D3DXMatrixIdentity(&identity);

	// Calculate Transformation Matrices
	D3DXMatrixLookAtLH(&view, &eye, &at, &up);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 1000.0f);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);
	D3DXMATRIX vp = view * proj;

	m_pSkinningEffect->SetMatrix("matVP", &vp);
	m_pSkinningEffect->SetVector("lightPos", &lightPos);
	m_pSkinningEffect->SetMatrix("matW", &identity);

	//Clear the viewport
	m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

	// Begin the scene
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		//Render Soldier
		{
			m_drone.Render(NULL, m_pSkinningEffect);
		}

		if (m_show > 0.0f)
		{
			RECT rc = { 0, 0, 800, 600 };
			m_pFont->DrawText(NULL, L"BANG!", -1, &rc, DT_CENTER | DT_VCENTER | DT_NOCLIP, 0xFF000000);
			SetRect(&rc, -5, -5, 795, 595);
			m_pFont->DrawText(NULL, L"BANG!", -1, &rc, DT_CENTER | DT_VCENTER | DT_NOCLIP, 0xFFFFFF00);
		}

		// End the scene
		m_pDevice->EndScene();
		m_pDevice->Present(0, 0, 0, 0);
	}
}

void Example5_2::OnCleanup()
{
	if (m_pSkinningEffect != NULL)
	{
		m_pSkinningEffect->Release();
		m_pSkinningEffect = NULL;
	}
}

void Example5_2::OnQuit()
{
}

void Example5_2::OnDeviceLost()
{
	m_pSkinningEffect->OnLostDevice();
}

void Example5_2::OnDeviceGained()
{
	m_pSkinningEffect->OnResetDevice();
}

void Example5_2::SetupCallback()
{
	//Get Animation controller
	m_pAnimController = m_drone.GetAnimController();

	//Get the "Aim" Animation set
	ID3DXKeyframedAnimationSet* animSet = NULL;
	m_pAnimController->GetAnimationSet(1, (ID3DXAnimationSet**)&animSet);

	//Compress the animation set
	ID3DXBuffer* compressedData = NULL;
	animSet->Compress(D3DXCOMPRESS_DEFAULT, 0.5f, NULL, &compressedData);

	//Create one callback key
	UINT numCallbacks = 1;
	D3DXKEY_CALLBACK keys[1];

	// GetSourceTicksPerSecond() returns the number
	// animation key frame ticks that occur per second.
	// Callback keyframe times are tick based.
	double ticks = animSet->GetSourceTicksPerSecond();

	// Set the first callback key to trigger a callback
	// half way through the animation sequence.
	keys[0].Time = float(animSet->GetPeriod() / 2.0f * ticks);
	keys[0].pCallbackData = (void*)this;
	
	//直接设置callback并不能触发callback，很奇怪
	//animSet->SetCallbackKey(0, keys);
	//m_pAnimController->SetTrackAnimationSet(0, animSet);
	//animSet->Release();

	// Create the ID3DXCompressedAnimationSet interface
	// with the callback keys.
	ID3DXCompressedAnimationSet* compressedAnimSet = NULL;
	D3DXCreateCompressedAnimationSet(animSet->GetName(),
		animSet->GetSourceTicksPerSecond(),
		animSet->GetPlaybackType(), compressedData,
		numCallbacks, keys, &compressedAnimSet);
	compressedData->Release();

	//Delete the old keyframed animation set.
	m_pAnimController->UnregisterAnimationSet(animSet);

	// and then add the new compressed animation set.
	m_pAnimController->RegisterAnimationSet(compressedAnimSet);

	// Hook up the animation set to the first track.
	m_pAnimController->SetTrackAnimationSet(0, compressedAnimSet);
	compressedAnimSet->Release();
}
