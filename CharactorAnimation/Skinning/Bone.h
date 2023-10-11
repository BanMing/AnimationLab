#pragma once
#include <d3dx9.h>

struct Bone :public D3DXFRAME
{
	//LPSTR Name; //Name of bone
	//D3DXMATRIX TransformationMatrix; //Local bone pos, rot & sca
	//LPD3DXMESHCONTAINER pMeshContainer; //Mesh connected to bone
	//D3DXFRAME* pFrameSibling; //Sibling bone pointer
	//D3DXFRAME* pFrameFirstChild; //First child bone
	D3DXMATRIX WorldTransformationMatrix;
};

void PrintSkeletonHierarchy(D3DXFRAME* bone);
void CalculateWorldMatrices(Bone* bone, D3DXMATRIX* parentMatrix);