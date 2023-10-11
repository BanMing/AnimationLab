#pragma once
#include<d3dx9.h>
#include<windows.h>

class BoneHierarchyLoader :public ID3DXAllocateHierarchy
{
public:
	HRESULT CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame) override;
	HRESULT CreateMeshContainer(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA* pMeshData,
		CONST D3DXMATERIAL* pMaterials,
		CONST D3DXEFFECTINSTANCE* pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD* pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER* ppNewMeshContainer) override;
	HRESULT DestroyFrame(LPD3DXFRAME pFrameToFree)override;
	HRESULT DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)override;
};

