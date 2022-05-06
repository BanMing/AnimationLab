#ifndef _H_CHAPTER10_SAMPLE02_
#define _H_CHAPTER10_SAMPLE02_

#include "Application.h"
#include "../Animation/Skeleton.h"
#include "../Animation/TClip.h"
#include "../Animation/Mesh.h"
#include "../Animation/Pose.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/Texture.h"
#include <vector>

struct AnimationInstance
{
	Pose mCurrentPose;
	Transform mTransform;
	std::vector<Mesh> mMeshes;
	float mCurPlayTime;
	int mCurClipIndex;
	std::vector <mat4> mPosePalette;
};

class Chapter10Sample02 :public Application
{
protected:
	AnimationInstance mCPUSkinAnimation;
	AnimationInstance mGPUSkinAnimation;
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mDynamicShader;
	Skeleton mSkeleton;
	std::vector<Clip> mClips;
	unsigned int mClipsNum;
	std::string mClipsNames;
	char* test;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void OnGUI();
	void Shutdown();
};

#endif
