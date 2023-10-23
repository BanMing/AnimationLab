#pragma once
#include <windows.h>
#include <d3dx9.h>

class BoneHierarchyLoader : public ID3DXAllocateHierarchy
{
public:
	HRESULT __stdcall CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame) override;
	HRESULT __stdcall CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials, const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER* ppNewMeshContainer) override;
	HRESULT __stdcall DestroyFrame(LPD3DXFRAME pFrameToFree) override;
	HRESULT __stdcall DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree) override;

};

