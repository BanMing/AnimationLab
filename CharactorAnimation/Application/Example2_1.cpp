#include "Example2_1.h"

Example2_1::Example2_1()
{
	m_appName = L"Example 2-1";
}

HRESULT Example2_1::OnInit()
{
	// Load Effect
	ID3DXBuffer* pErrorMsgs = NULL;
	HRESULT hRes = D3DXCreateEffectFromFile(m_pDevice, L"../resources/fx/lighting.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &pErrorMsgs);

	if (FAILED(hRes) && (pErrorMsgs != NULL))
	{
		MessageBox(NULL, (LPCWSTR)(char*)pErrorMsgs->GetBufferPointer(), L"Effect Error", MB_OK);
		return E_FAIL;
	}

	// Load Soldier Mesh
	m_soldier.Load(m_pDevice, L"../resources/meshes/soldier.x");
	return S_OK;
}

void Example2_1::OnUpdate(float deltaTime)
{
	// Camera Rotation 
	m_angle += deltaTime;
}

void Example2_1::OnRender()
{
	D3DXMATRIX identity, shadow;
	D3DXMatrixIdentity(&identity);

	//Set ground plane + light position
	D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 lightPos(-20.0f, 75.0f, -120.0f, 0.0f);

	//Create the shadow matrix
	D3DXMatrixShadow(&shadow, &lightPos, &ground);

	// Calculate Transformation Matrices
	D3DXMATRIX view, proj, world;
	D3DXMatrixIdentity(&world);
	D3DXVECTOR3 angleVec3 = D3DXVECTOR3(cos(m_angle) * 2.0f, 2.0f, sin(m_angle) * 2.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH(&view, &angleVec3, &up, &up);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);

	//Set transformation matrices
	m_pDevice->SetTransform(D3DTS_WORLD, &world);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);
	D3DXMATRIX vp = view * proj;
	m_pEffect->SetMatrix("matVP", &vp);

	// Clear the viewport
	m_pDevice->Clear(
		0,					// Num rectangles to clear
		NULL,				// Rectangles to clear (NULL = whole screen)
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,	// Clear the render target
		0xffffffff,			// Color AARRGGBB (in this case White) 
		1.0f,				// Clear Z-buffer to 1.0f
		0);					 //Clear Stencil Buffer to 0

	// Begin the scene
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		//Render Soldier
		{
			m_pEffect->SetMatrix("matW", &identity);
			m_pEffect->SetVector("lightPos", &lightPos);
			D3DXHANDLE hTech = m_pEffect->GetTechniqueByName("Lighting");
			m_pEffect->SetTechnique(hTech);
			m_pEffect->Begin(NULL, NULL);
			m_pEffect->BeginPass(0);

			m_soldier.Render();

			m_pEffect->EndPass();
			m_pEffect->End();
		}

		//Render Shadow
		{
			m_pEffect->SetMatrix("matW", &shadow);
			D3DXHANDLE hTech = m_pEffect->GetTechniqueByName("Shadow");
			m_pEffect->SetTechnique(hTech);
			m_pEffect->Begin(NULL, NULL);
			m_pEffect->BeginPass(0);

			m_soldier.Render();

			m_pEffect->EndPass();
			m_pEffect->End();
		}

		// End the scene.
		m_pDevice->EndScene();

		// Present the result (full screen)
		m_pDevice->Present(0, 0, 0, 0);
	}
}

void Example2_1::OnCleanup()
{
	if (m_pEffect != NULL)	m_pEffect->Release();
}

void Example2_1::OnQuit()
{
}

void Example2_1::OnDeviceLost()
{
	m_pEffect->OnLostDevice();
}

void Example2_1::OnDeviceGained()
{
	m_pEffect->OnResetDevice();
}
