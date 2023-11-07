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
	m_skinningType = SkinningType::CPU;
}

SkinnedMesh::~SkinnedMesh()
{
	BoneHierarchyLoader boneHierarchy;
	boneHierarchy.DestroyFrame(m_pRootBone);

	m_pDevice = NULL;
	m_pRootBone = NULL;
	m_pSphereMesh = NULL;
	m_pAnimControl = NULL;
}

void SkinnedMesh::Load(IDirect3DDevice9* pDevice, LPCWSTR fileName)
{
	Load(pDevice, fileName, SkinningType::CPU);
}

void SkinnedMesh::Load(IDirect3DDevice9* pDevice, LPCWSTR fileName, SkinningType skinningType)
{
	m_skinningType = skinningType;

	m_pDevice = pDevice;
	BoneHierarchyLoader boneHierarchy(m_skinningType);

	// Load a bone hierarchy from a file
	D3DXLoadMeshHierarchyFromX(fileName, D3DXMESH_MANAGED, m_pDevice, &boneHierarchy,
		NULL, &m_pRootBone, &m_pAnimControl);

	// Update all Bone transformation matrices
	D3DXMATRIX i;
	D3DXMatrixIdentity(&i);
	UpdateMatrices((Bone*)m_pRootBone, &i);

	// Create Sphere
	D3DXCreateSphere(m_pDevice, 0.02f, 10, 10, &m_pSphereMesh, NULL);
	SetupBoneMatrixPointers((Bone*)m_pRootBone);

}

void SkinnedMesh::Update(D3DXMATRIX world, float deltaTime)
{
	m_pAnimControl->AdvanceTime(deltaTime, NULL);
	UpdateMatrices((Bone*)m_pRootBone,&world);
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

void SkinnedMesh::Render(Bone* bone)
{
	Render(bone, NULL);
}

void SkinnedMesh::Render(Bone* bone, ID3DXEffect* pEffect)
{
	if (bone == NULL) { bone = (Bone*)m_pRootBone; }

	//If there is a mesh to render
	if (bone->pMeshContainer != NULL)
	{
		BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;
		if (boneMesh->pSkinInfo != NULL)
		{
			//Set up bone transforms
			int numBones = boneMesh->pSkinInfo->GetNumBones();
			for (int i = 0; i < numBones; i++)
			{
				D3DXMatrixMultiply(&boneMesh->currentBoneMatrices[i], &boneMesh->boneOffsetMatrices[i], boneMesh->boneMatrixPtrs[i]);
			}

			if (m_skinningType == SkinningType::CPU)
			{
				CPUSkinning(boneMesh);
			}
			else
			{
				GPUSkinning(boneMesh, pEffect);
			}

		}
		else
		{
			//Normal Static Mesh
			pEffect->SetMatrix("matW", &bone->WorldTransformationMatrix);

			D3DXHANDLE hTech;
			hTech = pEffect->GetTechniqueByName("Lighting");
			pEffect->SetTechnique(hTech);

			//Render the static mesh
			for (size_t i = 0; i < boneMesh->materials.size(); i++)
			{
				pEffect->SetTexture("texDiffuse", boneMesh->textures[i]);
				pEffect->Begin(NULL, NULL);
				pEffect->BeginPass(0);

				boneMesh->OriginalMesh->DrawSubset(i);

				pEffect->EndPass();
				pEffect->End();
			}
		}
	}

	//Render Siblings & Children
	if (bone->pFrameSibling != NULL) {
		Render((Bone*)bone->pFrameSibling, pEffect);
	}

	if (bone->pFrameFirstChild != NULL) {
		Render((Bone*)bone->pFrameFirstChild, pEffect);
	}

}

void SkinnedMesh::SetAnimation(std::string name)
{
	ID3DXAnimationSet* anim = NULL;

	int numAnims = (int)m_pAnimControl->GetMaxNumAnimationSets();

	for (int i = 0; i < numAnims; i++)
	{
		anim = NULL;
		m_pAnimControl->GetAnimationSet(i, &anim);

		if (anim != NULL)
		{
			if (strcmp(name.c_str(), anim->GetName()) == 0) 
			{
				m_pAnimControl->SetTrackAnimationSet(0, anim);
			}
			anim->Release();
		}
	}
}

void SkinnedMesh::GetAnimations(std::vector<std::string>& animations)
{
	ID3DXAnimationSet* anim = NULL;

	for (size_t i = 0; i < m_pAnimControl->GetMaxNumAnimationSets(); i++)
	{
		anim = NULL;
		m_pAnimControl->GetAnimationSet(i, &anim);

		if (anim != NULL)
		{
			animations.push_back(anim->GetName());
			anim->Release();
		}
	}
}

void SkinnedMesh::CPUSkinning(BoneMesh* boneMesh)
{
	//Update the skinned mesh
	BYTE* src = NULL, * dest = NULL;
	boneMesh->OriginalMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&src);
	boneMesh->MeshData.pMesh->LockVertexBuffer(0, (void**)&dest);

	boneMesh->pSkinInfo->UpdateSkinnedMesh(boneMesh->currentBoneMatrices, NULL, src, dest);

	boneMesh->OriginalMesh->UnlockVertexBuffer();
	boneMesh->MeshData.pMesh->UnlockVertexBuffer();

	//Render the mesh
	for (DWORD i = 0; i < boneMesh->NumAttributeGroups; i++)
	{
		int mtrl = boneMesh->attributeTable[i].AttribId;
		m_pDevice->SetMaterial(&(boneMesh->materials[mtrl]));
		m_pDevice->SetTexture(0, boneMesh->textures[mtrl]);
		boneMesh->MeshData.pMesh->DrawSubset(mtrl);
	}
}

void SkinnedMesh::GPUSkinning(BoneMesh* boneMesh, ID3DXEffect* pEffect)
{
	//Set HW matrix pelette
	pEffect->SetMatrixArray("FinalTransforms", boneMesh->currentBoneMatrices, boneMesh->pSkinInfo->GetNumBones());

	//Render the mesh
	for (DWORD i = 0; i < boneMesh->NumAttributeGroups; i++)
	{
		int mtrl = boneMesh->attributeTable[i].AttribId;
		m_pDevice->SetMaterial(&(boneMesh->materials[mtrl]));
		m_pDevice->SetTexture(0, boneMesh->textures[mtrl]);

		pEffect->SetTexture("texDiffuse", boneMesh->textures[mtrl]);
		D3DXHANDLE hTech = pEffect->GetTechniqueByName("Skinning");
		pEffect->SetTechnique(hTech);
		pEffect->Begin(NULL, NULL);
		pEffect->BeginPass(0);

		boneMesh->MeshData.pMesh->DrawSubset(mtrl);

		pEffect->EndPass();
		pEffect->End();
	}
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

void SkinnedMesh::SetupBoneMatrixPointers(Bone* bone)
{
	//Find all bones containing a mesh
	if (bone->pMeshContainer != NULL)
	{
		BoneMesh* boneMesh = (BoneMesh*)bone->pMeshContainer;

		//For the bones with skinned meshes, set up the pointers
		if (boneMesh->pSkinInfo != NULL)
		{
			//Get num bones influencing this mesh
			int NumBones = boneMesh->pSkinInfo->GetNumBones();

			//Create an array of pointers with numBones pointers
			boneMesh->boneMatrixPtrs = new D3DXMATRIX * [NumBones];

			//Fill array
			for (int i = 0; i < NumBones; i++)
			{
				//Find influencing bone by name
				Bone* b = (Bone*)D3DXFrameFind(m_pRootBone, boneMesh->pSkinInfo->GetBoneName(i));

				//and stroe pointer to it in the array
				if (b != NULL)
				{
					boneMesh->boneMatrixPtrs[i] = &b->WorldTransformationMatrix;
				}
				else
				{
					boneMesh->boneMatrixPtrs[i] = NULL;
				}
			}
		}
	}

	if (bone->pFrameSibling != NULL)
	{
		SetupBoneMatrixPointers((Bone*)bone->pFrameSibling);
	}

	if (bone->pFrameFirstChild != NULL)
	{
		SetupBoneMatrixPointers((Bone*)bone->pFrameFirstChild);
	}
}
