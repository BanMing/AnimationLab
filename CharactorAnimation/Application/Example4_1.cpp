#include "Example4_1.h"

static D3DXVECTOR3 eye{ 3.0f, 1.5f, 0.0f };
static D3DXVECTOR3 at{ 0.0f, 1.0f, 0.0f };
static D3DXVECTOR3 up{ 0.0f, 1.0f, 0.0f };
Example4_1::Example4_1()
{
	m_appName = L"Example4-1";
}

HRESULT Example4_1::OnInit()
{
	// Create new Aniamtion set
	D3DXCreateKeyframedAnimationSet("AniamtionSet1", 500, D3DXPLAY_PINGPONG, 1, 0, NULL, &m_pAnimSet);

	// Create keyframes
	D3DXKEY_VECTOR3 pos[3];
	pos[0].Time = 0.0f;
	pos[0].Value = D3DXVECTOR3(0.2f, 0.3f, 0.0f);
	pos[1].Time = 1000.0f;
	pos[1].Value = D3DXVECTOR3(0.8f, 0.5f, 0.0f);
	pos[2].Time = 2000.0f;
	pos[2].Value = D3DXVECTOR3(0.4f, 0.8f, 0.0f);

	D3DXKEY_VECTOR3 sca[2];
	sca[0].Time = 500.0f;
	sca[0].Value = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	sca[1].Time = 1500.0f;
	sca[1].Value = D3DXVECTOR3(4.8f, 4.5f, 4.0f);

	m_pAnimSet->RegisterAnimationSRTKeys("Animation1", 2, 0, 3, sca, NULL, pos, 0);

	D3DXCreateLine(m_pDevice, &m_pLine);
	return S_OK;
}

void Example4_1::OnUpdate(float deltaTime)
{
	m_time += deltaTime;
	if (m_time > m_pAnimSet->GetPeriod())
	{
		m_time -= (float)m_pAnimSet->GetPeriod();
	}
}

void Example4_1::OnRender()
{

	//Create Matrices
	D3DXMATRIX identity, world, view, proj, shadow;
	D3DXMatrixIdentity(&identity);
	D3DXMatrixLookAtLH(&view, &eye, &at, &up);
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 1.0f, 1000.0f);
	D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
	D3DXVECTOR4 lightPos(-50.0f, 75.0f, 50.0f, 0.0f);
	D3DXMatrixShadow(&shadow, &lightPos, &ground);

	m_pDevice->SetTransform(D3DTS_WORLD, &identity);
	m_pDevice->SetTransform(D3DTS_VIEW, &view);
	m_pDevice->SetTransform(D3DTS_PROJECTION, &proj);

	// Clear the viewport
	m_pDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0L);

	// Begin the scene 
	if (SUCCEEDED(m_pDevice->BeginScene()))
	{
		DrawLine();
		
		// End the scene.
		m_pDevice->EndScene();
		m_pDevice->Present(0, 0, 0, 0);
	}

}

void Example4_1::OnCleanup()
{
	m_pLine = NULL;
}

void Example4_1::OnQuit()
{
}

void Example4_1::OnDeviceLost()
{
	m_pLine->OnLostDevice();
}

void Example4_1::OnDeviceGained()
{
	m_pLine->OnResetDevice();
}

void Example4_1::DrawLine()
{
	D3DVIEWPORT9 vp;
	m_pDevice->GetViewport(&vp);

	// Get current frame data
	D3DXVECTOR3 pos, sca;
	D3DXQUATERNION rot;
	m_pAnimSet->GetSRT(m_time, 0, &sca, &rot, &pos);

	//Set current size
	float size = sca.x * 20.0f;
	m_pLine->SetWidth(size);

	m_pLine->Begin();

	//Scale to fit screen coordinates
	pos.x *= vp.Width;
	pos.y *= vp.Height;

	D3DXVECTOR2 p[] = { D3DXVECTOR2(pos.x - size * 0.5f,pos.y),
						D3DXVECTOR2(pos.x + size * 0.5f,pos.y) };

	m_pLine->Draw(p, 2, 0xffff0000);

	m_pLine->End();
}
