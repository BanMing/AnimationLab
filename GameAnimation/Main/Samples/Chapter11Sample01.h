#ifndef _H_CHAPTER11_SAMPLE01_
#define _H_CHAPTER11_SAMPLE01_

#include "Application.h"
#include "../Animation/TClip.h"
#include "../Animation/Skeleton.h"
#include "../Animation/Mesh.h"
#include "../OpenGL/Texture.h"
#include "../OpenGL/Shader.h"
#include <vector>

struct AnimationInstance
{
	Pose mCurPose;
	Transform mTransform;
	float mCurAnimationTime;
	int mCurAniamtionIndex;
	std::vector<Mesh> mMeshes;
	std::vector <mat4> mPosePalette;
};

class Chapter11Sample01 : public Application
{
	AnimationInstance mCPUAnimation;
	AnimationInstance mGPUAnimation;
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mDynamicShader;
	Skeleton mSkeleton;
	std::vector<FastClip> mClips;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();

};

#endif