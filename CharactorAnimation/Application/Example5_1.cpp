#include "Example5_1.h"
#include <string>
#pragma warning(disable:4996)

static D3DXVECTOR3 eye{ 0.0f, 1.5f, -3.0f };
static D3DXVECTOR3 at{ 0.0f, 1.0f, 0.0f };
static D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };

//Set ground plane + light position
static D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
static D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

std::string IntToString(int i)
{
	char num[10];
	_itoa(i, num, 10);
	return num;
}

Example5_1::Example5_1()
{
	m_appName = L"Example 5 - 1";
	m_pAnimController = NULL;
}

HRESULT Example5_1::OnInit()
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

	m_pAnimController = m_drone.GetAnimController();
	InitAnimationTrack();

	return S_OK;
}

void Example5_1::OnUpdate(float deltaTime)
{
	//Toggle Animation
	if (KeyDown(VK_RETURN))
	{
		Sleep(300);
		InitAnimationTrack();
	}
	m_pAnimController->AdvanceTime(deltaTime, NULL);
}

void Example5_1::OnRender()
{
	D3DXMATRIX identity, shadow, world, view, proj;
	//Create Matrices
	D3DXMatrixIdentity(&identity);

	//Create the shadow matrix
	D3DXMatrixShadow(&shadow, &lightPos, &ground);

	// Calculate Transformation Matrices
	D3DXMatrixLookAtLH(&view, &eye, &at, &up);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);
	D3DXMATRIX vp = view * proj;

	m_pSkinningEffect->SetMatrix("matVP", &vp);
	m_pSkinningEffect->SetMatrix("matW", &identity);
	m_pSkinningEffect->SetVector("lightPos", &lightPos);

	//Clear the viewport
	m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

	// Begin the scene
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		//Render Soldier
		{
			m_drone.Update(identity);
			m_drone.Render(NULL, m_pSkinningEffect);
		}

		DrawTrackStatus();
		if (KeyDown(VK_SPACE))
		{
			m_pDevice->Clear(0L, NULL, D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);
			m_drone.RenderSkeleton(NULL, NULL, identity);
		}

		// End the scene
		m_pDevice->EndScene();
		m_pDevice->Present(0, 0, 0, 0);
	}
}

void Example5_1::OnCleanup()
{
	if (m_pSkinningEffect != NULL)
	{
		m_pSkinningEffect->Release();
		m_pSkinningEffect = NULL;
	}
}

void Example5_1::OnQuit()
{
}

void Example5_1::OnDeviceLost()
{
	m_pSkinningEffect->OnLostDevice();
}

void Example5_1::OnDeviceGained()
{
	m_pSkinningEffect->OnResetDevice();
}

void Example5_1::InitAnimationTrack()
{
	srand(GetTickCount());

	//Reset the aniamtion controller's time
	m_pAnimController->ResetTime();

	//Get two random animations
	int numAnimations = m_pAnimController->GetMaxNumAnimationSets();
	ID3DXAnimationSet* anim1 = NULL;
	ID3DXAnimationSet* anim2 = NULL;
	m_pAnimController->GetAnimationSet(rand() % numAnimations, &anim1);
	m_pAnimController->GetAnimationSet(rand() % numAnimations, &anim2);

	//Assign them to two different tracks
	m_pAnimController->SetTrackAnimationSet(0, anim1);
	m_pAnimController->SetTrackAnimationSet(1, anim2);

	//Set random weight
	float w = (rand() % 100) / 1000.0f;
	m_pAnimController->SetTrackWeight(0, w);
	m_pAnimController->SetTrackWeight(1, w);

	//Set random speed (0 - 200%)
	m_pAnimController->SetTrackSpeed(0, (rand() % 1000) / 500.0f);
	m_pAnimController->SetTrackSpeed(1, (rand() % 1000) / 500.0f);

	//Set track proities
	m_pAnimController->SetTrackPriority(0, D3DXPRIORITY_HIGH);
	m_pAnimController->SetTrackPriority(1, D3DXPRIORITY_HIGH);

	//Enable tracks
	m_pAnimController->SetTrackEnable(0, true);
	m_pAnimController->SetTrackEnable(1, true);
}

void Example5_1::DrawTrackStatus()
{
	int numTracks = m_pAnimController->GetMaxNumTracks();

	for (int i = 0; i < numTracks; i++)
	{
		D3DXTRACK_DESC desc;
		ID3DXAnimationSet* anim = NULL;
		m_pAnimController->GetTrackDesc(i, &desc);
		m_pAnimController->GetTrackAnimationSet(i, &anim);

		std::string name = anim->GetName();
		while (name.size() < 10)name.push_back(' ');

		std::string s = std::string("Track #") + IntToString(i + 1) + name ;
		s += std::string("Weight = ") + IntToString((int)(desc.Weight * 100)) + "%";
		s += std::string(", Position = ") + IntToString((int)(desc.Position * 1000)) + " ms";
		s += std::string(", Speed = ") + IntToString((int)(desc.Speed * 100)) + "%";

		RECT r = { 10, 530 + i * 20, 0, 0 };
		m_pFont->DrawTextA(NULL, s.c_str(), -1, &r, DT_LEFT | DT_TOP | DT_NOCLIP, 0xAA000000);
	}

	RECT rc = { 10, 10, 0, 0 };
	m_pFont->DrawText(NULL, L"Press Return to randomize animations", -1, &rc, DT_LEFT | DT_TOP | DT_NOCLIP, 0x66000000);
}
