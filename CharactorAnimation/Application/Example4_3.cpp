#include "Example4_3.h"
static D3DXVECTOR3 eye{ 0.0f, 1.5f, -4.0f };
static D3DXVECTOR3 at{ 0.0f, 2.0f, 0.0f };
static D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };

//Set ground plane + light position
D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
D3DXVECTOR4 lightPos(-50.0f, 75.0f, -150.0f, 0.0f);

Example4_3::Example4_3()
{
	m_appName = L"Example 4-3";
	m_deltaTime = 0.0f;
	m_pSkinningEffect = NULL;
}

HRESULT Example4_3::OnInit()
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
	m_drone.Load(m_pDevice, L"../resources/meshes/soldier_4_3.x", SkinningType::GPU);

	std::vector<std::string> animations;
	m_drone.GetAnimations(animations);

	for (int i = 0; i < 4; i++)
	{
		D3DXMATRIX m;
		D3DXMatrixTranslation(&m, -1.5f + i * 1.0f, 1.0f, 0.0f);
		m_position.push_back(m);

		m_animControllers.push_back(m_drone.GetAnimController());
	}

	srand(GetTickCount());

	RandomizeAnimations();

	return S_OK;
}

void Example4_3::OnUpdate(float deltaTime)
{
	//Toggle Animation
	if (KeyDown(VK_RETURN))
	{
		Sleep(300);
		RandomizeAnimations();
	}

	m_deltaTime = deltaTime;
}

void Example4_3::OnRender()
{//Create Matrices
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


	//Clear the viewport
	m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

	// Begin the scene
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		//Render Soldier
		{
			for (int i = 0; i < 4; i++)
			{
				m_pSkinningEffect->SetMatrix("matW", &identity);
				m_animControllers[i]->AdvanceTime(m_deltaTime, NULL);
				m_drone.Update(m_position[i]);
				m_drone.Render(NULL, m_pSkinningEffect);
			}
		}

		// End the scene
		m_pDevice->EndScene();
		m_pDevice->Present(0, 0, 0, 0);
	}
}

void Example4_3::OnCleanup()
{
	if (m_pSkinningEffect != NULL)
	{
		m_pSkinningEffect->Release();
		m_pSkinningEffect = NULL;
	}
}

void Example4_3::OnQuit()
{
}

void Example4_3::OnDeviceLost()
{
	m_pSkinningEffect->OnLostDevice();
}

void Example4_3::OnDeviceGained()
{
	m_pSkinningEffect->OnResetDevice();
}

void Example4_3::RandomizeAnimations()
{
	for (size_t i = 0; i < m_animControllers.size(); i++)
	{
		int numAnimations = m_animControllers[i]->GetMaxNumAnimationSets();
		ID3DXAnimationSet* anim = NULL;
		m_animControllers[i]->GetAnimationSet(rand() % numAnimations, &anim);
		m_animControllers[i]->SetTrackAnimationSet(0, anim);
		anim->Release();
	}
}
