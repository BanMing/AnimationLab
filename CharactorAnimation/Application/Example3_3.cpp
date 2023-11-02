#include "Example3_3.h"

static D3DXVECTOR3 eye{ 0.0f, 1.5f, -3.0f };
static D3DXVECTOR3 at{ 0.0f, 1.0f, 0.0f };
static D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };

//Set ground plane + light position
static D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
static D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

Example3_3::Example3_3()
{
	m_appName = L"Example 3-3";
	m_time = 0.0f;
	m_pSkinningsEffect = NULL;
}

HRESULT Example3_3::OnInit()
{
	// Load Effect
	ID3DXBuffer* pErrorMsgs = NULL;
	HRESULT hRes = D3DXCreateEffectFromFile(m_pDevice, L"../resources/fx/skinning.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pSkinningsEffect, &pErrorMsgs);

	if (FAILED(hRes) && (pErrorMsgs != NULL))
	{
		MessageBox(NULL, (LPCWSTR)pErrorMsgs->GetBufferPointer(), L"Effect Error", MB_OK);
		return E_FAIL;
	}

	// Load Model
	m_drone.Load(m_pDevice, L"../resources/meshes/soldier_3_1.x", m_pSkinningsEffect);

	return S_OK;
}

void Example3_3::OnUpdate(float deltaTime)
{
	m_time += deltaTime;
}

void Example3_3::OnRender()
{
	//Create Matrices
	D3DXMATRIX identity, shadow, world, view, proj;
	D3DXMatrixIdentity(&identity);
	D3DXMatrixIdentity(&world);

	//Create the shadow matrix
	D3DXMatrixShadow(&shadow, &lightPos, &ground);

	// Calculate Transformation Matrices
	D3DXVECTOR3 angleVec3 = D3DXVECTOR3(cos(m_time) * 2.0f, 2.0f, sin(m_time) * 2.0f);
	D3DXMatrixLookAtLH(&view, &angleVec3, &up, &up);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	m_pDevice->SetTransform(D3DTS_WORLD, &world);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);
	D3DXMATRIX vp = view * proj;
	
	m_pSkinningsEffect->SetMatrix("matVP", &vp);
	m_pSkinningsEffect->SetMatrix("matW", &identity);
	m_pSkinningsEffect->SetVector("lightPos", &lightPos);

	//Clear the viewport
	m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

	// Begin the scene
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		//Render Soldier
		{
			m_drone.Render(NULL);
		}
		// End the scene
		m_pDevice->EndScene();
		m_pDevice->Present(0, 0, 0, 0);
	}
}

void Example3_3::OnCleanup()
{
}

void Example3_3::OnQuit()
{
}

void Example3_3::OnDeviceLost()
{
}

void Example3_3::OnDeviceGained()
{
}
