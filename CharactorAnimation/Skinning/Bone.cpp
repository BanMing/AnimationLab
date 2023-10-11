#include "Bone.h"
#include <iostream>


void PrintSkeletonHierarchy(D3DXFRAME* bone)
{
	printf("bone:%s", bone->Name);

	// Traverse Siblings
	if (bone->pFrameSibling != NULL)
	{
		PrintSkeletonHierarchy(bone->pFrameSibling);
	}

	// Traverse Children
	if (bone->pFrameFirstChild != NULL)
	{
		PrintSkeletonHierarchy(bone->pFrameFirstChild);
	}
}

void CalculateWorldMatrices(Bone* bone, D3DXMATRIX* parentMatrix)
{
	if (bone == NULL)return;

	// Calculate the world transformation matrix
	D3DXMatrixMultiply(&bone->WorldTransformationMatrix, &bone->TransformationMatrix, parentMatrix);

	// Perform the same calculation on all siblings...
	if (bone->pFrameSibling)
	{
		CalculateWorldMatrices((Bone*)bone->pFrameSibling, parentMatrix);
	}

	// all chilren
	if (bone->pFrameFirstChild)
	{
		// Note that we send a different parent matrix to siblings and children
		CalculateWorldMatrices((Bone*)bone->pFrameFirstChild, &bone->WorldTransformationMatrix);
	}
}
