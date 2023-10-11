#pragma once
#include <windows.h>
#include <d3dx9.h>
#include "Bone.h"
class SkinnedMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();
	void Load(IDirect3DDevice9* pDevice, LPCWSTR fileName);
	void RenderSkeleton(Bone* bone, Bone* parent, D3DXMATRIX world);
private:
	void UpdateMatrices(Bone* bone, D3DXMATRIX* parentMatrix);
	D3DXFRAME* m_pRootBone;
	IDirect3DDevice9* m_pDevice;
	LPD3DXMESH m_pSphereMesh;
};

