#include "Chapter14Sample01.h"
#include "../GLTF/GLTFLoader.h"
#include "../OpenGL/Uniform.h"

void Chapter14Sample01::Initialize()
{
	cgltf_data* data = LoadGLTFFile("Assets/dq.gltf");
	mMeshes = LoadMeshes(data);
	mSkeleton = LoadSkeleton(data);
	std::vector<FastClip> clips = LoadFastClips(data);
	mCurClip = clips[0];
	FreeGLTFFile(data);

	mLBSShader = new Shader("Shaders/skinned.vert", "Shaders/lit.frag");
	mDQShader = new Shader("Shaders/dualquaternion.vert", "Shaders/lit.frag");
	mDiffuseTexture = new Texture("Assets/dq.png");

	mCurPose = mSkeleton.GetRestPose();

	mCurPose.GetDualQuaternionPalette(mDqPosePalette);
	mSkeleton.GetInvBindPose(mDqInvBindPalette);

	mCurPose.GetMatrixPalette(mLbPosePalette);
	mLbInvBindPalette = mSkeleton.GetInvBindPose();

	mPlayTime = 0.0f;
	mBlendMode = 1;
}

void Chapter14Sample01::Update(float deltaTime)
{
	mPlayTime = mCurClip.Sample(mCurPose, mPlayTime + deltaTime);
	mCurPose.GetDualQuaternionPalette(mDqPosePalette);
	mCurPose.GetMatrixPalette(mLbPosePalette);
}

void Chapter14Sample01::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.0f, inAspectRatio, 0.01f, 1000.0f);
	mat4 view = lookAt(vec3(0, 3, 14), vec3(0, 0, 0), vec3(0, 1, 0));
	Transform model(vec3(0, 0, 0), quat(), vec3(1, 1, 1));

	if (mBlendMode == 0)
	{
		// Linear Blend
		mLBSShader->Bind();

		Uniform<mat4>::Set(mDQShader->GetUniform("model"), transformToMat4(model));
		Uniform<mat4>::Set(mLBSShader->GetUniform("view"), view);
		Uniform<mat4>::Set(mLBSShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(mLBSShader->GetUniform("light"), vec3(1, 1, 1));
		Uniform<mat4>::Set(mLBSShader->GetUniform("pose"), mLbPosePalette);
		Uniform<mat4>::Set(mLBSShader->GetUniform("invBindPose"), mLbInvBindPalette);

		mDiffuseTexture->Set(mLBSShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i)
		{
			mMeshes[i].Bind(mLBSShader->GetAttribute("position"), mLBSShader->GetAttribute("normal"), mLBSShader->GetAttribute("texCoord"), mLBSShader->GetAttribute("weights"), mLBSShader->GetAttribute("joints"));
			mMeshes[i].Draw();
			mMeshes[i].UnBind(mLBSShader->GetAttribute("position"), mLBSShader->GetAttribute("normal"), mLBSShader->GetAttribute("texCoord"), mLBSShader->GetAttribute("weights"), mLBSShader->GetAttribute("joints"));
		}
		mDiffuseTexture->UnSet(0);
		mLBSShader->UnBind();
	}
	else
	{
		// Dual Quaternion
		mDQShader->Bind();
		Uniform<mat4>::Set(mDQShader->GetUniform("model"), transformToMat4(model));
		Uniform<mat4>::Set(mDQShader->GetUniform("view"), view);
		Uniform<mat4>::Set(mDQShader->GetUniform("projection"), projection);
		Uniform<vec3>::Set(mDQShader->GetUniform("light"), vec3(1, 1, 1));
		Uniform<DualQuaternion>::Set(mDQShader->GetUniform("pose"), mDqPosePalette);
		Uniform<DualQuaternion>::Set(mDQShader->GetUniform("invBindPose"), mDqInvBindPalette);

		mDiffuseTexture->Set(mDQShader->GetUniform("tex0"), 0);
		for (unsigned int i = 0, size = (unsigned int)mMeshes.size(); i < size; ++i)
		{
			mMeshes[i].Bind(mDQShader->GetAttribute("position"), mDQShader->GetAttribute("normal"), mDQShader->GetAttribute("texCoord"), mDQShader->GetAttribute("weights"), mDQShader->GetAttribute("joints"));
			mMeshes[i].Draw();
			mMeshes[i].UnBind(mDQShader->GetAttribute("position"), mDQShader->GetAttribute("normal"), mDQShader->GetAttribute("texCoord"), mDQShader->GetAttribute("weights"), mDQShader->GetAttribute("joints"));
		}
		mDiffuseTexture->UnSet(0);
		mDQShader->UnBind();
	}
}

void Chapter14Sample01::OnGUI()
{
	ImGui::Begin("Dual Quaternion");
	const char* kBlendTypes[] = { "Linear Blend","Dual Quaternion Blend" };
	bool isChanged = ImGui::Combo("Blend Mode", &mBlendMode, kBlendTypes, IM_ARRAYSIZE(kBlendTypes));
	if (isChanged)
	{
		// Need to reset so positions is in the buffers
		for (size_t i = 0, size = mMeshes.size(); i < size; ++i)
		{
			mMeshes[i].UpdateOpenGLBuffers();
		}
	}
	ImGui::End();
}

void Chapter14Sample01::Shutdown()
{
	delete mLBSShader;
	delete mDQShader;
	delete mDiffuseTexture;
}
