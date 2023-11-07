#include "Example4_2.h"
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>

static D3DXVECTOR3 eye{ 0.0f, 1.5f, -3.0f };
static D3DXVECTOR3 at{ 0.0f, 1.0f, 0.0f };
static D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };

//Set ground plane + light position
static D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
static D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

Example4_2::Example4_2()
{
	m_appName = L"Example 4-2";
	m_time = 0.0f;
	m_pSkinningEffect = NULL;
}

HRESULT Example4_2::OnInit()
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
	m_drone.Load(m_pDevice, L"../resources/meshes/soldier_3_1.x", SkinningType::GPU);

	m_drone.SetAnimation("Walk");
	return S_OK;
}

void Example4_2::OnUpdate(float deltaTime)
{
	m_time += deltaTime;
	D3DXMATRIX world;
	D3DXMatrixRotationY(&world, m_time);
	m_drone.Update(world, deltaTime);
}

void Example4_2::OnRender()
{
	//Create Matrices
	D3DXMATRIX identity, shadow, world, view, proj;
	D3DXMatrixIdentity(&identity);

	//Create the shadow matrix
	D3DXMatrixShadow(&shadow, &lightPos, &ground);

	// Calculate Transformation Matrices
	D3DXVECTOR3 angleVec3 = D3DXVECTOR3(cos(m_time) * 2.0f, 2.0f, sin(m_time) * 2.0f);
	D3DXMatrixLookAtLH(&view, &angleVec3, &up, &up);
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
			m_drone.Render(NULL, m_pSkinningEffect);
		}

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

void Example4_2::OnCleanup()
{
	if (m_pSkinningEffect != NULL)
	{
		m_pSkinningEffect->Release();
		m_pSkinningEffect = NULL;
	}
}

void Example4_2::OnQuit()
{
}

void Example4_2::OnDeviceLost()
{
	m_pSkinningEffect->OnLostDevice();
}

void Example4_2::OnDeviceGained()
{
	m_pSkinningEffect->OnResetDevice();
}
