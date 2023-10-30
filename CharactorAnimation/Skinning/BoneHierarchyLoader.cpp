#include "BoneHierarchyLoader.h"
#include "Bone.h"
#include "BoneMesh.h"

#pragma warning(disable:4996)

HRESULT __stdcall BoneHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME* ppNewFrame)
{
	Bone* newBone = new Bone;
	memset(newBone, 0, sizeof(Bone));

	//Copy name
	if (Name != NULL)
	{
		newBone->Name = new char[strlen(Name) + 1];
		strcpy(newBone->Name, Name);
	}

	//Set the transformation matrices
	D3DXMatrixIdentity(&newBone->TransformationMatrix);
	D3DXMatrixIdentity(&newBone->WorldTransformationMatrix);

	//Return the new bone...
	*ppNewFrame = (D3DXFRAME*)newBone;

	return S_OK;
}

HRESULT __stdcall BoneHierarchyLoader::CreateMeshContainer(LPCSTR Name, const D3DXMESHDATA* pMeshData, const D3DXMATERIAL* pMaterials, const D3DXEFFECTINSTANCE* pEffectInstances, DWORD NumMaterials, const DWORD* pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER* ppNewMeshContainer)
{
	//Create new Bone Mesh
	BoneMesh* boneMesh = (BoneMesh*)malloc(sizeof(BoneMesh));
	memset(boneMesh, 0, sizeof(BoneMesh));

	//Get mesh data
	boneMesh->OriginalMesh = pMeshData->pMesh;
	boneMesh->MeshData.pMesh = pMeshData->pMesh;
	boneMesh->MeshData.Type = pMeshData->Type;

	//Add Refernece so the mesh is not deallocated
	pMeshData->pMesh->AddRef();

	IDirect3DDevice9* pDevice = NULL;
	pMeshData->pMesh->GetDevice(&pDevice);

	// Copy materials and load textures (just like with a static mesh)
	for (DWORD i = 0; i < NumMaterials; i++)
	{
		D3DXMATERIAL mtrl;
		memcpy(&mtrl, &pMaterials[i], sizeof(D3DXMATERIAL));
		boneMesh->materials.push_back(mtrl.MatD3D);
		IDirect3DTexture9* newTexture = NULL;

		if (mtrl.pTextureFilename != NULL)
		{
			char textureFname[200];
			strcpy(textureFname, "../resources/meshes/");
			strcat(textureFname, mtrl.pTextureFilename);
			WCHAR res[256];
			MultiByteToWideChar(CP_ACP, 0, textureFname, strlen(textureFname) + 1, res, sizeof(res) / sizeof(res[0]));
			// Load texture
			D3DXCreateTextureFromFile(pDevice, res, &newTexture);
		}

		boneMesh->textures.push_back(newTexture);
	}

	if (pSkinInfo != NULL)
	{
		//Get Skin Info
		boneMesh->pSkinInfo = pSkinInfo;

		//Add reference so SkinInfo isn't deallocated
		pSkinInfo->AddRef();


		if (m_skinningType == SkinningType::CPU)
		{
			//Clone mesh and store in boneMesh->MeshData.pMesh
			pMeshData->pMesh->CloneMeshFVF(D3DXMESH_MANAGED, pMeshData->pMesh->GetFVF(), pDevice, &boneMesh->MeshData.pMesh);
		}
		else
		{
			DWORD maxVerInfluences = 0;
			DWORD numBoneComboEntries = 0;
			ID3DXBuffer* boneComboTable = 0;

			pSkinInfo->ConvertToIndexedBlendedMesh(pMeshData->pMesh, D3DXMESH_MANAGED | D3DXMESH_WRITEONLY, 30,
				NULL, NULL, NULL, NULL,
				&maxVerInfluences, &numBoneComboEntries, &boneComboTable, &boneMesh->MeshData.pMesh);

			if (boneComboTable!=NULL)
			{
				boneComboTable->Release();
			}
		}

		//Get Attribute Table
		boneMesh->MeshData.pMesh->GetAttributeTable(NULL, &boneMesh->NumAttributeGroups);

		boneMesh->attributeTable = new D3DXATTRIBUTERANGE[boneMesh->NumAttributeGroups];
		boneMesh->MeshData.pMesh->GetAttributeTable(boneMesh->attributeTable, NULL);

		//Create bone offset and current matrices
		int NumBones = pSkinInfo->GetNumBones();
		boneMesh->boneOffsetMatrices = new D3DXMATRIX[NumBones];
		boneMesh->currentBoneMatrices = new D3DXMATRIX[NumBones];

		//Get bone offset matrices
		for (int i = 0; i < NumBones; i++)
		{
			boneMesh->boneOffsetMatrices[i] = *(boneMesh->pSkinInfo->GetBoneOffsetMatrix(i));
		}
	}

	//Set ppNewMeshContainer to the newly created boneMesh container
	*ppNewMeshContainer = boneMesh;
	return S_OK;
}

HRESULT __stdcall BoneHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{
	if (pFrameToFree)
	{
		//Free Name String
		if (pFrameToFree->Name != NULL)
			delete[] pFrameToFree->Name;

		//Free Frame
		delete pFrameToFree;
	}

	pFrameToFree = NULL;

	return S_OK;
}

HRESULT __stdcall BoneHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	BoneMesh* boneMesh = (BoneMesh*)pMeshContainerToFree;

	//Release textures
	int numTextures = (int)boneMesh->textures.size();
	for (int i = 0; i < numTextures; i++)
		if (boneMesh->textures[i] != NULL)
			boneMesh->textures[i]->Release();

	//Release mesh data
	if (boneMesh->MeshData.pMesh)boneMesh->MeshData.pMesh->Release();
	if (boneMesh->pSkinInfo)boneMesh->pSkinInfo->Release();
	if (boneMesh->OriginalMesh)boneMesh->OriginalMesh->Release();
	delete boneMesh;

	return S_OK;
}
