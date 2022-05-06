#ifndef _H_CHAPTER10_SAMPLE02_
#define _H_CHAPTER10_SAMPLE02_

#include "Application.h"
#include "../Animation/Skeleton.h"
#include "../Animation/TClip.h"
#include "../Animation/Mesh.h"
#include "../Animation/Pose.h"
#include "../OpenGL/DebugDraw.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/Texture.h"
#include <vector>

enum SkinType
{
	CPU,
	GPU
};

struct AnimationInstance
{
	Pose mCurrentPose;
	float mCurPlayTime;
	int mCurClipIndex;
	std::vector <mat4> mPosePalette;

	bool mIsShowCurPose;
	bool mIsShowBindPose;
	bool mIsShowRestPose;

	int mCurrentSkinType;
};

class Chapter10Sample02 :public Application
{
protected:
	AnimationInstance mAnimationIns;
	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mDynamicShader;
	Skeleton mSkeleton;

	bool mIsShowMesh;
	std::vector<Mesh> mMeshes;
	
	std::vector<Clip> mClips;
	std::string mClipsNames;

	DebugDraw* mRestPoseDraw;
	DebugDraw* mBindPoseDraw;
	DebugDraw* mCurPoseDraw;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void OnGUI();
	void Shutdown();
};

#endif
