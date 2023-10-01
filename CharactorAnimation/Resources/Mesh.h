#pragma once
#include <vector>
#include <d3dx9.h>
#include <fstream>

class Mesh
{
public:
	Mesh();
	Mesh(IDirect3DDevice9* pDevice, char fName[]);
	~Mesh();
	HRESULT Load(IDirect3DDevice9* pDevice, LPCWSTR fName);
	void Render();
	void Release();
private:
	ID3DXMesh* m_pMesh;
	std::vector<IDirect3DTexture9*>m_textures;
	std::vector<D3DMATERIAL9> m_materials;
	D3DMATERIAL9 m_white;
	IDirect3DDevice9* m_pDevice;
};

