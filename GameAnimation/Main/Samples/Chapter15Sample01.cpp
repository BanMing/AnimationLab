#include "Chapter15Sample01.h"

#include "../Animation/AnimBaker.h"
#include "../GLTF/GLTFLoader.h"
#include "../Math/mat4.h"
#include "../OpenGL/Uniform.h"

#include <io.h>
void Chapter15Sample01::Initialize()
{
	mCrowdShader = new Shader("Shaders/crowd.vert", "Shaders/lit.frag");

	cgltf_data* data = LoadGLTFFile("Assets/Woman.gltf");
	mSkeleton = LoadSkeleton(data);
	mMeshes = LoadMeshes(data);
	mClips = LoadFastClips(data);
	FreeGLTFFile(data);

	mDiffuseTexture = new Texture("Assets/Woman.png");

	unsigned int numClips = (unsigned int) mClips.size();
	mCrowdTextures.resize(numClips);
	mCrowds.resize(numClips);
	for (unsigned int i = 0; i < numClips; ++i)
	{
		std::string fileName = "Assets/AnimTex/";
		fileName += mClips[i].GetName();
		fileName += ".animTex";
		bool fileExists = _access(fileName.c_str(), 0) == 0;
		if (fileExists)
		{
			mCrowdTextures[i].Load(fileName.c_str());
		}
		else
		{
			mCrowdTextures[i].Resize(512);
			BakeAnimationToTexture(mSkeleton, mClips[i], mCrowdTextures[i]);
			mCrowdTextures[i].Save(fileName.c_str());
		}
	}
}

void Chapter15Sample01::Update(float deltaTime)
{
}

void Chapter15Sample01::Render(float inAspectRatio)
{
	mat4 projection = perspective(60.f, inAspectRatio, 0.f, 100.f);
	mat4 view = lookAt(vec3(0, 15, 40), vec3(0, 3, 0), vec3(0, 1, 0));

	mCrowdShader->Bind();

	int viewUniform = mCrowdShader->GetUniform("view");
	Uniform<mat4>::Set(viewUniform, view);

	int projUniform = mCrowdShader->GetUniform("projection");
	Uniform<mat4>::Set(projUniform, projection);

	int lightUniform = mCrowdShader->GetUniform("light");
	Uniform<vec3>::Set(lightUniform, vec3(1, 1, 1));

	int invBind = mCrowdShader->GetUniform("invBindPose");
	Uniform<mat4>::Set(invBind, mSkeleton.GetInvBindPose());

	int texUniform = mCrowdShader->GetUniform("tex0");
	mDiffuseTexture->Set(texUniform, 0);

	//int animTexUniform = mCrowdShader->GetUniform("animTex");
	//mCrowdTexture->Set(animTexUniform, 1);
	//mCrowds.SetUniforms(mCrowdShader);

	//int pAttrib = mCrowdShader->GetAttribute("position");
	//int nAttrib = mCrowdShader->GetAttribute("normal");
	//int tAttrib = mCrowdShader->GetAttribute("texCoord");
	//int wAttrib = mCrowdShader->GetAttribute("weights");
	//int jAttrib = mCrowdShader->GetAttribute("joints");

	//mMesh.Bind(pAttrib, nAttrib, tAttrib, wAttrib, jAttrib);
	//mMesh.DrawInstanced(mCrowd.Size());
	//mMesh.UnBind(pAttrib, nAttrib, tAttrib, wAttrib, jAttrib);

	//mCrowdTexture->UnSet(1);
	//mDiffuseTexture->UnSet(0);
	//mCrowdShader->UnBind();
}

void Chapter15Sample01::OnGUI()
{
}

void Chapter15Sample01::Shutdown()
{
}
