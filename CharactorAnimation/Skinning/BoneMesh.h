#pragma once
#include <d3dx9.h>
#include <vector>

struct BoneMesh : public D3DXMESHCONTAINER
{
	ID3DXMesh* OriginalMesh;
	std::vector<D3DMATERIAL9> materials;
	std::vector<IDirect3DTexture9*> textures;

	DWORD NumAttributeGroups;
	D3DXATTRIBUTERANGE* attributeTable;
	D3DXMATRIX** boneMatrixPtrs;
	D3DXMATRIX* boneOffsetMatrices;
	D3DXMATRIX* currentBoneMatrices;
};

