#include "SkinnedMesh.h"
#include "BoneHierarchyLoader.h"
struct VERTEX
{
	D3DXVECTOR3 position;
	D3DCOLOR color;
	VERTEX(D3DXVECTOR3 pos, D3DCOLOR col) :position(pos), color(col) {}
};
SkinnedMesh::SkinnedMesh()
{
	m_pDevice = NULL;
	m_pRootBone = NULL;
	m_pSphereMesh = NULL;
}

SkinnedMesh::~SkinnedMesh()
{
	BoneHierarchyLoader boneHierarchy;
	boneHierarchy.DestroyFrame(m_pRootBone);

	m_pDevice = NULL;
	m_pRootBone = NULL;
	m_pSphereMesh = NULL;
}

void SkinnedMesh::Load(IDirect3DDevice9* pDevice, LPCWSTR fileName)
{
	m_pDevice = pDevice;
	BoneHierarchyLoader boneHierarchy;

	// Load a bone hierarchy from a file
	D3DXLoadMeshHierarchyFromX(fileName, D3DXMESH_MANAGED, m_pDevice, &boneHierarchy,
		NULL, &m_pRootBone, NULL);

	// Update all Bone transformation matrices
	D3DXMATRIX i;
	D3DXMatrixIdentity(&i);
	UpdateMatrices((Bone*)m_pRootBone, &i);

	// Create Sphere
	D3DXCreateSphere(m_pDevice, 0.02f, 10, 10, &m_pSphereMesh, NULL);
}

void SkinnedMesh::RenderSkeleton(Bone* bone, Bone* parent, D3DXMATRIX world)
{
	// Temporary function to render the bony hierarchy
	if (bone == NULL)
	{
		bone = (Bone*)m_pRootBone;
	}

	D3DXMATRIX r, s;
	D3DXMatrixRotationYawPitchRoll(&r, -D3DX_PI * 0.5f, 0.0f, 0.0f);

	// Draw line between bones
	if (parent != NULL && bone->Name != NULL && parent->Name != NULL)
	{
		// Draw Sphere
		m_pDevice->SetRenderState(D3DRS_LIGHTING, true);
		D3DMATRIX worldTrans = r * bone->WorldTransformationMatrix * world;
		m_pDevice->SetTransform(D3DTS_WORLD, &worldTrans);
		m_pSphereMesh->DrawSubset(0);

		D3DXMATRIX w1 = bone->WorldTransformationMatrix;
		D3DXMATRIX w2 = parent->WorldTransformationMatrix;

		//Extract translation
		D3DXVECTOR3 thisBone = D3DXVECTOR3(w1(3, 0), w1(3, 1), w1(3, 2));
		D3DXVECTOR3 ParentBone = D3DXVECTOR3(w2(3, 0), w2(3, 1), w2(3, 2));
		D3DXVECTOR3 len = thisBone - ParentBone;
		if (D3DXVec3Length(&len) < 2.0f)
		{
			m_pDevice->SetTransform(D3DTS_WORLD, &world);
			VERTEX vert[] = { VERTEX(ParentBone, 0xffff0000), VERTEX(thisBone, 0xff00ff00) };
			m_pDevice->SetRenderState(D3DRS_LIGHTING, false);
			m_pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
			m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 1, &vert[0], sizeof(VERTEX));
		}
	}

	if (bone->pFrameSibling)RenderSkeleton((Bone*)bone->pFrameSibling, parent, world);
	if (bone->pFrameFirstChild)RenderSkeleton((Bone*)bone->pFrameFirstChild, bone, world);
}

void SkinnedMesh::UpdateMatrices(Bone* bone, D3DXMATRIX* parentMatrix)
{
	if (bone == NULL)return;

	//Calculate the combined transformation matrix
	D3DXMatrixMultiply(&bone->WorldTransformationMatrix,
		&bone->TransformationMatrix,
		parentMatrix);

	//Perform the same calculation on all siblings
	if (bone->pFrameSibling)
	{
		UpdateMatrices((Bone*)bone->pFrameSibling,
			parentMatrix);
	}

	//... and all children
	if (bone->pFrameFirstChild)
	{
		UpdateMatrices((Bone*)bone->pFrameFirstChild,
			&bone->WorldTransformationMatrix);
	}
}
