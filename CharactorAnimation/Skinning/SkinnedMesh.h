#pragma once
#include <windows.h>
#include <d3dx9.h>
#include "Bone.h"
#include "BoneMesh.h"

enum SkinningType
{
	GPU,
	CPU
};

class SkinnedMesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();
	void Load(IDirect3DDevice9* pDevice, LPCWSTR fileName);
	void Load(IDirect3DDevice9* pDevice, LPCWSTR fileName, SkinningType skinningType);
	void RenderSkeleton(Bone* bone, Bone* parent, D3DXMATRIX world);
	void Render(Bone* bone);
	void Render(Bone* bone, ID3DXEffect* pEffect);
private:
	void CPUSkinning(BoneMesh* boneMesh);
	void GPUSkinning(BoneMesh* boneMesh, ID3DXEffect* pEffect);
	void UpdateMatrices(Bone* bone, D3DXMATRIX* parentMatrix);
	void SetupBoneMatrixPointers(Bone* bone);
	D3DXFRAME* m_pRootBone;
	IDirect3DDevice9* m_pDevice;
	LPD3DXMESH m_pSphereMesh;
	SkinningType m_skinningType;
};

