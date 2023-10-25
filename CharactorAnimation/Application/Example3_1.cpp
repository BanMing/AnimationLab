#include "Example3_1.h"

static D3DXVECTOR3 g_eyePos{ 0.0f, 1.5f, -3.0f };
static D3DXVECTOR3 g_atPos{ 0.0f, 1.0f, 0.0f };
static D3DXVECTOR3 g_upPos{ 0.0f, 1.0f, 0.0f };

Example3_1::Example3_1()
{
	m_appName = L"Example 3-1";
	m_time = 0.0f;
}

HRESULT Example3_1::OnInit()
{
	m_drone.Load(m_pDevice, L"../resources/meshes/soldier_3_1.x");
	return S_OK;
}

void Example3_1::OnUpdate(float deltaTime)
{
	m_time += deltaTime;
}

void Example3_1::OnRender()
{
	//Create Matrices
	D3DXMATRIX identity, world, view, proj;
	D3DXMatrixIdentity(&identity);
	D3DXMatrixRotationY(&world, m_time);
	D3DXMatrixLookAtLH(&view, &g_eyePos, &g_atPos, &g_upPos);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 1000.0f);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	// Clear the viewport
	m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

	// Begin the scene 
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		m_drone.RenderSkeleton(NULL, NULL, world);

		// End the scene.
		m_pDevice->EndScene();
		m_pDevice->Present(0, 0, 0, 0);
	}
}

void Example3_1::OnCleanup()
{
}

void Example3_1::OnQuit()
{
}

void Example3_1::OnDeviceLost()
{
}

void Example3_1::OnDeviceGained()
{
}