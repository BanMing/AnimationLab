#include "Chapter10Sample02.h"
#include "../GLTF/GLTFLoader.h"
#include "../OpenGL/Draw.h"
#include "../OpenGL/Uniform.h"

void Chapter10Sample02::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);
	mMeshes = LoadMeshes(data);
	mClips = LoadAnimationClips(data);
	FreeGLTFFile(data);

	mCurrentWalkPose = mSkeleton.GetRestPose();

	for (unsigned int i = 0; i < mClips.size(); i++)
	{
		if (mClips[i].GetName() == "Walking")
		{
			mWalkClipIndex = i;
		}
		else if (mClips[i].GetName() == "Running")
		{
			mRunClipIndex = i;
		}
	}

	mDiffuseTexture = new Texture("Assets/Woman.png");
	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mWalkTransform.position = vec3(2, 0, 0);
}

void Chapter10Sample02::Update(float inDeltaTime)
{
	mWalkPlayTime = mClips[mWalkClipIndex].Sample(mCurrentWalkPose, inDeltaTime + mWalkPlayTime);
	mMeshes[0].CPUSkin(mSkeleton, mCurrentWalkPose);
}

void Chapter10Sample02::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model;

	// CPU Skinned Mesh
	model = transformToMat4(mWalkTransform);
	mStaticShader->Bind();
	Uniform<mat4>::Set(mStaticShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->GetUniform("light"), vec3(1, 1, 1));

	mDiffuseTexture->Set(mStaticShader->GetUniform("tex0"), 0);

	mMeshes[0].Bind(mStaticShader->GetAttribute("position"),
					mStaticShader->GetAttribute("normal"),
					mStaticShader->GetAttribute("texCoord"), -1, -1);
	mMeshes[0].Draw();
	mMeshes[0].UnBind(mStaticShader->GetAttribute("position"),
					  mStaticShader->GetAttribute("normal"),
					  mStaticShader->GetAttribute("texCoord"), -1, -1);

	mDiffuseTexture->UnSet(0);
	mStaticShader->UnBind();
}

void Chapter10Sample02::Shutdown()
{
	mClips.clear();
	mMeshes.clear();

	delete mDiffuseTexture;
}