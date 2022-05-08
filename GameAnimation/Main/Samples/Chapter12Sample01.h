#ifndef _H_CHAPTER12_SAMPLE01_
#define _H_CHAPTER12_SAMPLE01_

#include "Application.h"
#include "../Animation/TClip.h"
#include "../Animation/Pose.h"
#include "../Animation/Skeleton.h"
#include "../Animation/Mesh.h"
#include "../OpenGL/DebugDraw.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/Texture.h"
#include "../Math/mat4.h"
#include <vector>
#include <string>

struct AnimationInstance
{
	int mCurClipIndex;
	int mPlayTime;
	Pose mCurPose;
	bool mIsShowPose;
	DebugDraw* mPoseDraw;
};

class Chapter12Sample01 :public Application
{
protected:
	AnimationInstance mAnimationInsA;
	AnimationInstance mAnimationInsB;

	std::vector<FastClip> mClips;
	std::vector<Mesh> mMeshes;
	Skeleton mSkeleton;
	std::vector<mat4> mPoseMatrixPalette;
	int mSkinningType;
	float mBlendRate;
	int mBlendRoot;
	Pose mBlendPose;

	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mDynamicShader;

	DebugDraw* mRestPoseDraw;
	DebugDraw* mBindPoseDraw;
	DebugDraw* mBlendPoseDraw;

	bool mIsShowMesh;
	bool mIsShowBindPose;
	bool mIsShowRestPose;
	bool mIsShowBlendPose;

	std::string mClipNames;

public:
	void Initialize() override;
	void Update(float inDeltaTime) override;
	void Render(float inAspectRatio) override;
	void OnGUI() override;
	void Shutdown() override;
};

#endif