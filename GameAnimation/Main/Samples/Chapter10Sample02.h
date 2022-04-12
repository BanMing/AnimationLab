#ifndef _H_CHAPTER10_SAMPLE02_
#define _H_CHAPTER10_SAMPLE02_

#include "Application.h"
#include "../Animation/Skeleton.h"
#include "../Animation/Clip.h"
#include "../Animation/Mesh.h"
#include "../Animation/Pose.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/Texture.h"
#include <vector>

class Chapter10Sample02 :public Application
{
protected:
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Transform mWalkTransform;
	Skeleton mSkeleton;
	std::vector<Mesh> mMeshes;
	std::vector<Clip> mClips;
	Pose mCurrentWalkPose;
	float mWalkPlayTime;
	float mRunPlayTime;
	unsigned int mWalkClipIndex;
	unsigned int mRunClipIndex;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif
