#include "Chapter10Sample02.h"
#include "../GLTF/GLTFLoader.h"
#include "../OpenGL/Draw.h"
#include "../OpenGL/Uniform.h"
#include "../3rd/glad/glad.h"

void Chapter10Sample02::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);
	mMeshes = LoadMeshes(data);
	mClips = LoadAnimationClips(data);
	FreeGLTFFile(data);

	mIsShowMesh = true;
	mAnimationIns.mCurrentPose = mSkeleton.GetRestPose();

	for (unsigned int i = 0; i < mClips.size(); i++)
	{
		std::string& clipName = mClips[i].GetName();
		mClipsNames += clipName + '\0';
		mClips[i].SetLooping(false);
	}

	mDiffuseTexture = new Texture("Assets/Woman.png");
	mStaticShader = new Shader("Shaders/static.vert", "Shaders/lit.frag");
	mDynamicShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");

	mAnimationIns.mCurrentSkinType = SkinType::GPU;

	mRestPoseDraw = new DebugDraw();
	mRestPoseDraw->FromPose(mSkeleton.GetRestPose());
	mRestPoseDraw->UpdateOpenGLBuffers();

	mBindPoseDraw = new DebugDraw();
	mBindPoseDraw->FromPose(mSkeleton.GetBindPose());
	mBindPoseDraw->UpdateOpenGLBuffers();

	mCurPoseDraw = new DebugDraw();
	mCurPoseDraw->FromPose(mSkeleton.GetRestPose());
	mCurPoseDraw->UpdateOpenGLBuffers();

}

void Chapter10Sample02::Update(float inDeltaTime)
{
	mAnimationIns.mCurPlayTime = mClips[mAnimationIns.mCurClipIndex].Sample(mAnimationIns.mCurrentPose,
																			inDeltaTime + mAnimationIns.mCurPlayTime);
	if (mIsShowMesh)
	{
		if (mAnimationIns.mCurrentSkinType == SkinType::CPU)
		{
			for (size_t i = 0; i < mMeshes.size(); i++)
			{
				mMeshes[i].CPUSkin(mSkeleton, mAnimationIns.mCurrentPose);
			}
		}
		else
		{
			mAnimationIns.mCurrentPose.GetMatrixPalette(mAnimationIns.mPosePalette);
		}
	}

	if (mAnimationIns.mIsShowCurPose)
	{
		mCurPoseDraw->FromPose(mAnimationIns.mCurrentPose);
	}
}

void Chapter10Sample02::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 5, 7), vec3(0, 3, 0), vec3(0, 1, 0));
	mat4 mvp = projection * view; // No model

	if (mIsShowMesh)
	{
		Shader* shader = mStaticShader;
		if (mAnimationIns.mCurrentSkinType == SkinType::GPU)
		{
			shader = mDynamicShader;
		}

		shader->Bind();
		Uniform<mat4>::Set(shader->GetUniform("model"), mat4());
		Uniform<mat4>::Set(shader->GetUniform("view"), view);
		Uniform<mat4>::Set(shader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(shader->GetUniform("light"), vec3(1, 1, 1));

		if (mAnimationIns.mCurrentSkinType == SkinType::GPU)
		{
			Uniform<mat4>::Set(shader->GetUniform("pose"), mAnimationIns.mPosePalette);
			Uniform<mat4>::Set(shader->GetUniform("invBindPose"), mSkeleton.GetInvBindPose());
		}

		mDiffuseTexture->Set(shader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i)
		{
			int weights = -1;
			int influences = -1;
			if (mAnimationIns.mCurrentSkinType == SkinType::GPU)
			{
				weights = shader->GetAttribute("weights");
				influences = shader->GetAttribute("joints");
			}

			mMeshes[i].Bind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), weights, influences);
			mMeshes[i].Draw();
			mMeshes[i].UnBind(shader->GetAttribute("position"), shader->GetAttribute("normal"), shader->GetAttribute("texCoord"), weights, influences);
		}
		mDiffuseTexture->UnSet(0);
		shader->UnBind();
	}

	glDisable(GL_DEPTH_TEST);

	if (mAnimationIns.mIsShowBindPose)
	{
		mBindPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 1, 0), mvp);
	}

	if (mAnimationIns.mIsShowCurPose)
	{
		mCurPoseDraw->Draw(DebugDrawMode::Lines, vec3(0, 0, 1), mvp);
	}

	if (mAnimationIns.mIsShowRestPose)
	{
		mRestPoseDraw->Draw(DebugDrawMode::Lines, vec3(1, 0, 0), mvp);
	}
	glEnable(GL_DEPTH_TEST);
}

void Chapter10Sample02::OnGUI()
{
	ImGui::Begin("Skin Animation");
	ImGui::Combo("Clip", &mAnimationIns.mCurClipIndex, mClipsNames.c_str());

	const char* kSkinTypes[] = { "CPU","GPU" };
	ImGui::Combo("Skin", &mAnimationIns.mCurrentSkinType, kSkinTypes, IM_ARRAYSIZE(kSkinTypes));

	const float progress = mAnimationIns.mCurPlayTime / mClips[mAnimationIns.mCurClipIndex].GetDuration();
	ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f), "");
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Progress");

	ImGui::Checkbox("Show Mesh", &mIsShowMesh);
	ImGui::Checkbox("Show Current Pose", &mAnimationIns.mIsShowCurPose);
	ImGui::Checkbox("Show Rest Pose", &mAnimationIns.mIsShowRestPose);
	ImGui::Checkbox("Show Bind Pose", &mAnimationIns.mIsShowBindPose);

	ImGui::End();
}

void Chapter10Sample02::Shutdown()
{
	mClips.clear();
	mClips.shrink_to_fit();

	mClipsNames.clear();
	mClipsNames.shrink_to_fit();

	mMeshes.clear();
	mMeshes.shrink_to_fit();

	delete mDiffuseTexture;
	delete mStaticShader;
	delete mDynamicShader;

	delete mRestPoseDraw;
	delete mBindPoseDraw;
	delete mCurPoseDraw;
}