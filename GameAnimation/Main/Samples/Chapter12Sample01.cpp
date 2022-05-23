#include "Chapter12Sample01.h"
#include "../3rd/cgltf/cgltf.h"
#include "../3rd/glad/glad.h"
#include "../GLTF/GLTFLoader.h"
#include "../Animation/Blending.h"
#include "../OpenGL/Uniform.h"

void Chapter12Sample01::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mMeshes = LoadMeshes(data);
	mSkeleton = LoadSkeleton(data);
	mClips = LoadFastClips(data);
	FreeGLTFFile(data);

	mAnimationInsA.mCurPose = mSkeleton.GetRestPose();
	mAnimationInsA.mPoseDraw = new DebugDraw();
	mAnimationInsA.mPoseDraw->FromPose(mSkeleton.GetRestPose());
	mAnimationInsA.mPoseDraw->UpdateOpenGLBuffers();

	mAnimationInsB.mCurPose = mSkeleton.GetRestPose();
	mAnimationInsB.mPoseDraw = new DebugDraw();
	mAnimationInsB.mPoseDraw->FromPose(mSkeleton.GetRestPose());
	mAnimationInsB.mPoseDraw->UpdateOpenGLBuffers();

	mSkinningType = SkinningType::CPU;
	mBlendRate = 0.5f;
	mBlendPose = mSkeleton.GetRestPose();

	mDiffuseTexture = new Texture("Assets/Woman.png");

	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mDynamicShader = new Shader("Shaders/preskinned.vert", "Shaders/lit.frag");

	mRestPoseDraw = new DebugDraw();
	mRestPoseDraw->FromPose(mSkeleton.GetRestPose());
	mRestPoseDraw->UpdateOpenGLBuffers();

	mBindPoseDraw = new DebugDraw();
	mBindPoseDraw->FromPose(mSkeleton.GetBindPose());
	mBindPoseDraw->UpdateOpenGLBuffers();

	mBlendPoseDraw = new DebugDraw();
	mBlendPoseDraw->FromPose(mSkeleton.GetRestPose());
	mBlendPoseDraw->UpdateOpenGLBuffers();

	mIsShowMesh = true;
	mBlendRoot = 0;

	for (size_t i = 0; i < mClips.size(); i++)
	{
		mClipNames += mClips[i].GetName() + '\0';
	}

	//mJointNames = "Root\0";
	for (size_t i = 0; i < mSkeleton.GetJointNames().size(); i++)
	{
		mJointNames += mSkeleton.GetJointName(i) + '\0';
	}
}

void Chapter12Sample01::Update(float inDeltaTime)
{
	mAnimationInsA.mPlayTime = mClips[mAnimationInsA.mCurClipIndex].Sample(mAnimationInsA.mCurPose,
																		   inDeltaTime + mAnimationInsA.mPlayTime);
	if (mAnimationInsA.mIsShowPose)
	{
		mAnimationInsA.mPoseDraw->FromPose(mAnimationInsA.mCurPose);
		mAnimationInsA.mPoseDraw->UpdateOpenGLBuffers();
	}

	mAnimationInsB.mPlayTime = mClips[mAnimationInsB.mCurClipIndex].Sample(mAnimationInsB.mCurPose,
																		   inDeltaTime + mAnimationInsB.mPlayTime);
	if (mAnimationInsB.mIsShowPose)
	{
		mAnimationInsB.mPoseDraw->FromPose(mAnimationInsB.mCurPose);
		mAnimationInsB.mPoseDraw->UpdateOpenGLBuffers();
	}

	Blend(mBlendPose, mAnimationInsA.mCurPose, mAnimationInsB.mCurPose, mBlendRate, mBlendRoot);
	if (mIsShowBlendPose)
	{
		mBlendPoseDraw->FromPose(mBlendPose);
		mBlendPoseDraw->UpdateOpenGLBuffers();
	}

	if (mIsShowMesh)
	{
		mBlendPose.GetMatrixPalette(mPoseMatrixPalette);
		for (size_t i = 0; i < mPoseMatrixPalette.size(); i++)
		{
			mPoseMatrixPalette[i] = mPoseMatrixPalette[i] * mSkeleton.GetInvBindPose()[i];
		}

		if (mSkinningType == SkinningType::CPU)
		{

			for (size_t i = 0; i < mMeshes.size(); i++)
			{
				mMeshes[i].CPUSKin(mPoseMatrixPalette);
			}
		}
	}
}

void Chapter12Sample01::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 4, 7), vec3(0, 4, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	if (mIsShowMesh)
	{
		Shader* shader = mStaticShader;
		if (mSkinningType == SkinningType::GPU)
		{
			shader = mDynamicShader;
		}

		shader->Bind();
		Uniform<mat4>::Set(shader->GetUniform("model"), mat4());
		Uniform<mat4>::Set(shader->GetUniform("view"), view);
		Uniform<mat4>::Set(shader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(shader->GetUniform("light"), vec3(1, 1, 1));

		if (mSkinningType == SkinningType::GPU)
		{
			Uniform<mat4>::Set(shader->GetUniform("animated"), mPoseMatrixPalette);
		}

		mDiffuseTexture->Set(shader->GetUniform("tex0"), 0);
		for (size_t i = 0; i < mMeshes.size(); i++)
		{
			int weights = -1;
			int influences = -1;
			if (mSkinningType == SkinningType::GPU)
			{
				weights = shader->GetAttribute("weights");
				influences = shader->GetAttribute("joints");
			}

			mMeshes[i].Bind(shader->GetAttribute("position"),
							shader->GetAttribute("normal"),
							shader->GetAttribute("texCoord"),
							weights, influences);
			mMeshes[i].Draw();
			mMeshes[i].UnBind(shader->GetAttribute("position"),
							  shader->GetAttribute("normal"),
							  shader->GetAttribute("texCoord"),
							  weights, influences);

		}
		mDiffuseTexture->UnSet(0);
		shader->UnBind();
	}

	glDisable(GL_DEPTH_TEST);

	if (mAnimationInsA.mIsShowPose)
	{
		mAnimationInsA.mPoseDraw->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	}

	if (mAnimationInsB.mIsShowPose)
	{
		mAnimationInsB.mPoseDraw->Draw(DebugDrawMode::Lines, vec3(1, 1, 0), mvp);
	}

	if (mIsShowBlendPose)
	{
		mBlendPoseDraw->Draw(DebugDrawMode::Lines, vec3(1, 1, 1), mvp);
	}

	if (mIsShowBindPose)
	{
		mBindPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	}

	if (mIsShowRestPose)
	{
		mRestPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	}

	glEnable(GL_DEPTH_TEST);
}

void Chapter12Sample01::OnGUI()
{
	ImGui::Begin("Blend Animation");

	bool isChanged = ImGui::Combo("Aniamtion A", &mAnimationInsA.mCurClipIndex, mClipNames.c_str());
	if (isChanged)
	{
		mAnimationInsA.mCurPose = mSkeleton.GetRestPose();
	}

	float progressA = mAnimationInsA.mPlayTime / mClips[mAnimationInsA.mCurClipIndex].GetDuration();
	ImGui::ProgressBar(progressA, ImVec2(0, 0), "");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("A Progress");

	isChanged = ImGui::Combo("Aniamtion B", &mAnimationInsB.mCurClipIndex, mClipNames.c_str());
	if (isChanged)
	{
		mAnimationInsB.mCurPose = mSkeleton.GetRestPose();
	}

	float progressB = mAnimationInsB.mPlayTime / mClips[mAnimationInsB.mCurClipIndex].GetDuration();
	ImGui::ProgressBar(progressB, ImVec2(0, 0), "");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("A Progress");

	const char* kSkinTypes[] = { "CPU","GPU" };
	isChanged = ImGui::Combo("Skin", &mSkinningType, kSkinTypes, IM_ARRAYSIZE(kSkinTypes));
	if (isChanged)
	{
		// Need to reset so positions is in the buffers
		for (size_t i = 0, size = mMeshes.size(); i < size; ++i)
		{
			mMeshes[i].UpdateOpenGLBuffers();
		}
	}

	isChanged = ImGui::Combo("Blend Root", &mBlendRoot, mJointNames.c_str());

	ImGui::SliderFloat("Blend Rate", &mBlendRate, 0.0f, 1.0f);

	ImGui::Checkbox("Show Mesh", &mIsShowMesh);
	ImGui::Checkbox("Show Bind Pose", &mIsShowBindPose);
	ImGui::Checkbox("Show Rest Pose", &mIsShowRestPose);
	ImGui::Checkbox("Show Blended Pose", &mIsShowBlendPose);
	ImGui::Checkbox("Show Pose A", &mAnimationInsA.mIsShowPose);
	ImGui::Checkbox("Show Pose B", &mAnimationInsB.mIsShowPose);

	ImGui::End();
}

void Chapter12Sample01::Shutdown()
{
	delete mAnimationInsA.mPoseDraw;
	delete mAnimationInsB.mPoseDraw;

	mClips.clear();
	mClips.shrink_to_fit();

	mMeshes.clear();
	mMeshes.shrink_to_fit();

	mPoseMatrixPalette.clear();
	mPoseMatrixPalette.shrink_to_fit();

	delete mDiffuseTexture;
	delete mStaticShader;
	delete mDynamicShader;

	delete mRestPoseDraw;
	delete mBindPoseDraw;
	delete mBlendPoseDraw;

	mClipNames.clear();
	mClipNames.shrink_to_fit();
}
