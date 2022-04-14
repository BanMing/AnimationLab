#include "Chapter10Sample02.h"
#include "../GLTF/GLTFLoader.h"
#include "../OpenGL/Draw.h"
#include "../OpenGL/Uniform.h"

void Chapter10Sample02::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);
	std::vector<Mesh> meshes = LoadMeshes(data);
	mClips = LoadAnimationClips(data);
	FreeGLTFFile(data);

	mCPUSkinAnimation.mMeshes = meshes;
	mCPUSkinAnimation.mCurrentPose = mSkeleton.GetRestPose();

	mGPUSkinAnimation.mMeshes = meshes;
	mGPUSkinAnimation.mCurrentPose = mSkeleton.GetRestPose();

	for (unsigned int i = 0; i < mClips.size(); i++)
	{
		if (mClips[i].GetName() == "Punch")
		{
			mCPUSkinAnimation.mCurClipIndex = i;
		}
		
		if (mClips[i].GetName() == "Punch")
		{
			mGPUSkinAnimation.mCurClipIndex = i;
		}
	}

	mDiffuseTexture = new Texture("Assets/Woman.png");

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mCPUSkinAnimation.mTransform.position = vec3(-2, 0, 0);

	mDynamicShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mGPUSkinAnimation.mTransform.position = vec3(2, 0, 0);
}

void Chapter10Sample02::Update(float inDeltaTime)
{
	mCPUSkinAnimation.mCurPlayTime = mClips[mCPUSkinAnimation.mCurClipIndex].Sample(mCPUSkinAnimation.mCurrentPose,
																					inDeltaTime + mCPUSkinAnimation.mCurPlayTime);
	for (size_t i = 0; i < mCPUSkinAnimation.mMeshes.size(); i++)
	{
		mCPUSkinAnimation.mMeshes[i].CPUSkin(mSkeleton, mCPUSkinAnimation.mCurrentPose);
	}

	mGPUSkinAnimation.mCurPlayTime = mClips[mGPUSkinAnimation.mCurClipIndex].Sample(mGPUSkinAnimation.mCurrentPose,
																					inDeltaTime + mGPUSkinAnimation.mCurPlayTime);

	mGPUSkinAnimation.mCurrentPose.GetMatrixPalette(mGPUSkinAnimation.mPosePalette);
}

void Chapter10Sample02::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model;

	// CPU Skinned Mesh
	model = transformToMat4(mCPUSkinAnimation.mTransform);
	mStaticShader->Bind();
	Uniform<mat4>::Set(mStaticShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->GetUniform("light"), vec3(1, 1, 1));

	mDiffuseTexture->Set(mStaticShader->GetUniform("tex0"), 0);
	for (size_t i = 0; i < mCPUSkinAnimation.mMeshes.size(); i++)
	{
		mCPUSkinAnimation.mMeshes[i].Bind(mStaticShader->GetAttribute("position"),
										  mStaticShader->GetAttribute("normal"),
										  mStaticShader->GetAttribute("texCoord"), -1, -1);
		mCPUSkinAnimation.mMeshes[i].Draw();
		mCPUSkinAnimation.mMeshes[i].UnBind(mStaticShader->GetAttribute("position"),
											mStaticShader->GetAttribute("normal"),
											mStaticShader->GetAttribute("texCoord"), -1, -1);
	}
	mDiffuseTexture->UnSet(0);
	mStaticShader->UnBind();

	// GPU Skinned Mesh
	model = transformToMat4(mGPUSkinAnimation.mTransform);
	mDynamicShader->Bind();
	Uniform<mat4>::Set(mDynamicShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mDynamicShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mDynamicShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mDynamicShader->GetUniform("light"), vec3(1, 1, 1));

	Uniform<mat4>::Set(mDynamicShader->GetUniform("pose"), mGPUSkinAnimation.mPosePalette);
	Uniform<mat4>::Set(mDynamicShader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());

	mDiffuseTexture->Set(mDynamicShader->GetUniform("tex0"), 0);
	for (size_t i = 0; i < mGPUSkinAnimation.mMeshes.size(); i++)
	{
		mGPUSkinAnimation.mMeshes[i].Bind(mDynamicShader->GetAttribute("position"),
										  mDynamicShader->GetAttribute("normal"),
										  mDynamicShader->GetAttribute("texCoord"),
										  mDynamicShader->GetAttribute("weights"),
										  mDynamicShader->GetAttribute("joints"));
		mGPUSkinAnimation.mMeshes[i].Draw();
		mGPUSkinAnimation.mMeshes[i].UnBind(mDynamicShader->GetAttribute("position"),
											mDynamicShader->GetAttribute("normal"),
											mDynamicShader->GetAttribute("texCoord"),
											mDynamicShader->GetAttribute("weights"),
											mDynamicShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mDynamicShader->UnBind();

}

void Chapter10Sample02::Shutdown()
{
	mClips.clear();
	mCPUSkinAnimation.mMeshes.clear();
	mGPUSkinAnimation.mMeshes.clear();

	delete mDiffuseTexture;
	delete mStaticShader;
	delete mDynamicShader;
}