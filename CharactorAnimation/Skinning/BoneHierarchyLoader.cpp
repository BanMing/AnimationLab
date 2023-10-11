#include "Bone.h"
#include "BoneMesh.h"
#include "BoneHierarchyLoader.h"

#pragma warning(disable:4996)

HRESULT BoneHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)
{
	Bone* newBone = new Bone();
	memset(newBone, 0, sizeof(Bone));

	// Copy Name
	if (Name != NULL)
	{
		newBone->Name = new char[strlen(Name) + 1];
		strcpy(newBone->Name, Name);
	}

	// Set the transformation matrices
	D3DXMatrixIdentity(&newBone->TransformationMatrix);
	D3DXMatrixIdentity(&newBone->WorldTransformationMatrix);

	*ppNewFrame = (D3DXFRAME*)newBone;

	return S_OK;
}

HRESULT BoneHierarchyLoader::CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials, const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER* ppNewMeshContainer)
{
	//Just return a temporary mesh for now...
	*ppNewMeshContainer = new BoneMesh();
	memset(*ppNewMeshContainer, 0, sizeof(BoneMesh));
	return S_OK;
}

HRESULT BoneHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	if (pFrameToFree)
	{
		// Free Name String
		if (pFrameToFree->Name != NULL)
		{
			delete[] pFrameToFree->Name;

			// Free Frame
			delete pFrameToFree;
		}
	}

	pFrameToFree = NULL;
	return S_OK;
}

HRESULT BoneHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	if (pMeshContainerToFree != NULL)
		delete pMeshContainerToFree;

	return S_OK;
}
