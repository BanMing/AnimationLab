#include "Chapter11Sample01.h"
#include "../GLTF/GLTFLoader.h"
#include "../Animation/RearrangeBones.h"
#include "../OpenGL/Uniform.h"

void Chapter11Sample01::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);
	std::vector<Clip>clips = LoadAnimationClips(data);
	std::vector<Mesh> meshes = LoadMeshes(data);
	FreeGLTFFile(data);

	BoneMap boneMap = RearrangeSkeleton(mSkeleton);

	mGPUAnimation.mCurPose = mSkeleton.GetRestPose();
	mCPUAnimation.mCurPose = mSkeleton.GetRestPose();

	for (size_t i = 0; i < meshes.size(); i++)
	{
		RearrangeMesh(meshes[i], boneMap);
	}
	mGPUAnimation.mMeshes = meshes;
	mCPUAnimation.mMeshes = meshes;

	mClips.resize(clips.size());
	for (size_t i = 0; i < clips.size(); i++)
	{
		mClips[i] = OptimizeClip(clips[i]);
		RearrangeFastclip(mClips[i], boneMap);

		if (mClips[i].GetName() == "Punch")
		{
			mGPUAnimation.mCurAniamtionIndex = i;
		}

		if (mClips[i].GetName() == "Punch")
		{
			mCPUAnimation.mCurAniamtionIndex = i;
		}
	}

	mCPUAnimation.mTransform.position = vec3(-2, 0, 0);
	mGPUAnimation.mTransform.position = vec3(2, 0, 0);

	mDiffuseTexture = new Texture("Assets/Woman.png");

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mDynamicShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");
}

void Chapter11Sample01::Update(float inDeltaTime)
{
	mGPUAnimation.mCurAnimationTime = mClips[mGPUAnimation.mCurAniamtionIndex].Sample(mGPUAnimation.mCurPose,
																					  mGPUAnimation.mCurAnimationTime +
																					  inDeltaTime);
	mGPUAnimation.mCurPose.GetMatrixPalette(mGPUAnimation.mPosePalette);
	for (size_t i = 0; i < mGPUAnimation.mPosePalette.size(); i++)
	{
		mGPUAnimation.mPosePalette[i] = mGPUAnimation.mPosePalette[i] * mSkeleton.GetInvBindPose()[i];
	}

	mCPUAnimation.mCurAnimationTime = mClips[mCPUAnimation.mCurAniamtionIndex].Sample(mCPUAnimation.mCurPose,
																					  mCPUAnimation.mCurAnimationTime +
																					  inDeltaTime);
	mCPUAnimation.mCurPose.GetMatrixPalette(mCPUAnimation.mPosePalette);

	for (size_t i = 0; i < mCPUAnimation.mPosePalette.size(); i++)
	{
		mCPUAnimation.mPosePalette[i] = mCPUAnimation.mPosePalette[i] * mSkeleton.GetInvBindPose()[i];
	}

	for (size_t i = 0; i < mCPUAnimation.mMeshes.size(); i++)
	{
		mCPUAnimation.mMeshes[i].CPUSKin(mCPUAnimation.mPosePalette);
	}
}

void Chapter11Sample01::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 model;

	// CPU skin
	model = transformToMat4(mCPUAnimation.mTransform);
	mStaticShader->Bind();
	Uniform<mat4>::Set(mStaticShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mStaticShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mStaticShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mStaticShader->GetUniform("light"), vec3(1, 1, 1));

	mDiffuseTexture->Set(mStaticShader->GetUniform("tex0"), 0);
	for (size_t i = 0; i < mCPUAnimation.mMeshes.size(); i++)
	{
		mCPUAnimation.mMeshes[i].Bind(mStaticShader->GetAttribute("position"),
									  mStaticShader->GetAttribute("normal"),
									  mStaticShader->GetAttribute("texCoord"), -1, -1);
		mCPUAnimation.mMeshes[i].Draw();
		mCPUAnimation.mMeshes[i].UnBind(mStaticShader->GetAttribute("position"),
										mStaticShader->GetAttribute("normal"),
										mStaticShader->GetAttribute("texCoord"), -1, -1);
	}
	mDiffuseTexture->UnSet(0);
	mStaticShader->UnBind();

	// GPU skin
	model = transformToMat4(mGPUAnimation.mTransform);
	mDynamicShader->Bind();
	Uniform<mat4>::Set(mDynamicShader->GetUniform("model"), model);
	Uniform<mat4>::Set(mDynamicShader->GetUniform("view"), view);
	Uniform<mat4>::Set(mDynamicShader->GetUniform("projection"), projection);
	Uniform<vec3>::Set(mDynamicShader->GetUniform("light"), vec3(1, 1, 1));

	Uniform<mat4>::Set(mDynamicShader->GetUniform("animated"), mGPUAnimation.mPosePalette);

	mDiffuseTexture->Set(mDynamicShader->GetUniform("tex0"), 0);
	for (size_t i = 0; i < mCPUAnimation.mMeshes.size(); i++)
	{
		mGPUAnimation.mMeshes[i].Bind(mDynamicShader->GetAttribute("position"),
									  mDynamicShader->GetAttribute("normal"),
									  mDynamicShader->GetAttribute("texCoord"),
									  mDynamicShader->GetAttribute("weights"),
									  mDynamicShader->GetAttribute("joints"));
		mGPUAnimation.mMeshes[i].Draw();
		mGPUAnimation.mMeshes[i].UnBind(mDynamicShader->GetAttribute("position"),
										mDynamicShader->GetAttribute("normal"),
										mDynamicShader->GetAttribute("texCoord"),
										mDynamicShader->GetAttribute("weights"),
										mDynamicShader->GetAttribute("joints"));
	}
	mDiffuseTexture->UnSet(0);
	mDynamicShader->UnBind();

}

void Chapter11Sample01::Shutdown()
{
	mClips.clear();
	mCPUAnimation.mMeshes.clear();
	mGPUAnimation.mMeshes.clear();

	delete mDiffuseTexture;
	delete mStaticShader;
	delete mDynamicShader;
}
