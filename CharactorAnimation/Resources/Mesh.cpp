#include "Mesh.h"

#pragma warning(disable:4996)

#define PATH_TO_TEXTURES "../resources/meshes/"

Mesh::Mesh()
{
	m_pMesh = NULL;
	m_pDevice = NULL;
}

Mesh::Mesh(IDirect3DDevice9* pDevice, char fName[])
{
	m_pMesh = NULL;
	m_pDevice = pDevice;
	Load(pDevice, (LPCWSTR)fName);
}

Mesh::~Mesh()
{
	Release();
}

HRESULT Mesh::Load(IDirect3DDevice9* pDevice, LPCWSTR fName)
{
	// Release old resources
	Release();

	m_pDevice = pDevice;

	//Set m_white material
	m_white.Ambient = m_white.Specular = m_white.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_white.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	m_white.Power = 1.0f;

	// Load new Mesh
	ID3DXBuffer* adjacencyBfr = NULL;
	ID3DXBuffer* materialBfr = NULL;
	DWORD noMaterials = NULL;

	if (FAILED(D3DXLoadMeshFromX(fName, D3DXMESH_MANAGED, m_pDevice, &adjacencyBfr, &materialBfr, NULL, &noMaterials, &m_pMesh)))
	{
		printf("Failed to load mesh...\n");
		return E_FAIL;
	}

	D3DXMATERIAL* mtrls = (D3DXMATERIAL*)materialBfr->GetBufferPointer();

	for (int i = 0; i < (int)noMaterials; i++)
	{
		if (mtrls[i].pTextureFilename != NULL)
		{
			m_materials.push_back(mtrls[i].MatD3D);

			if (mtrls[i].pTextureFilename != NULL)
			{
				char textureFileName[90];
				strcpy(textureFileName, PATH_TO_TEXTURES);
				strcat(textureFileName, mtrls[i].pTextureFilename);
				IDirect3DTexture9* newTexture = NULL;
				WCHAR res[256];
				MultiByteToWideChar(CP_ACP, 0, textureFileName, strlen(textureFileName) + 1, res, sizeof(res) / sizeof(res[0]));
				D3DXCreateTextureFromFile(m_pDevice, res, &newTexture);
				m_textures.push_back(newTexture);
			}
			else
			{
				m_textures.push_back(NULL);
			}
		}
	}
	m_pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_COMPACT | D3DXMESHOPT_VERTEXCACHE, (DWORD*)adjacencyBfr->GetBufferPointer(), NULL, NULL, NULL);

	adjacencyBfr->Release();
	materialBfr->Release();
	return S_OK;
}

void Mesh::Render()
{
	for (DWORD i = 0; i < (DWORD)m_materials.size(); i++)
	{
		// Set material
		if (m_textures[i] != NULL)
		{
			m_pDevice->SetMaterial(&m_white);
		}
		else
		{
			m_pDevice->SetMaterial(&m_materials[i]);
		}

		// Set texture
		m_pDevice->SetTexture(0, m_textures[i]);

		// Render mesh
		m_pMesh->DrawSubset(i);
	}
}

void Mesh::Release()
{
	// Clear old mesh..
	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}

	// Clear textures and materials
	for (size_t i = 0; i < m_textures.size(); i++)
	{
		if (m_textures[i] != NULL)
		{
			m_textures[i]->Release();
		}
	}

	m_textures.clear();
	m_materials.clear();

	m_pDevice = NULL;
}