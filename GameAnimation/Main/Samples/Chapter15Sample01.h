#ifndef _H_CHAPTER15_SAMPLE01_
#define _H_CHAPTER15_SAMPLE01_
#include "../Animation/AnimTexture.h"
#include "../Animation/Crowd.h"
#include "../Animation/Mesh.h"
#include "../Animation/Skeleton.h"
#include "../Animation/TClip.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/Texture.h"
#include "Application.h"

#include <vector>
class Chapter15Sample01 : public Application
{
public:
	void Initialize();
	void Update(float deltaTime);
	void Render(float inAspectRatio);
	void OnGUI();
	void Shutdown();

protected:
	void SetCrowdSize(unsigned int size);

protected:
	Shader* mCrowdShader;
	Skeleton mSkeleton;
	Texture* mDiffuseTexture;

	std::vector<AnimTexture> mCrowdTextures;
	std::vector<Crowd> mCrowds;
	std::vector<Mesh> mMeshes;

	std::vector<FastClip> mClips;
};
#endif