/******************************************************************
** @File         : Chapter14Sample03.h
** @Author       : BanMing
** @Date         : 2022/06/28
** @Description  :
*******************************************************************/

#ifndef _H_CHAPTER13_SAMPLE03_
#define _H_CHAPTER13_SAMPLE03_
#include "Application.h"
#include "../OpenGL/DebugDraw.h"
#include "../OpenGL/Texture.h"
#include "../Animation/Mesh.h"
#include "../Animation/TClip.h"
#include "../IK/IKLeg.h"

class Chapter13Sample03 :public Application
{
protected:
	Texture* mCourseTexture;
	std::vector<Mesh> mIKGround;
	std::vector<Triangle> mTriangles;

	VectorTrack mMotionTrack;
	float mWalkingTime;

	IKLeg* mLeftLeg;
	IKLeg* mRightLeg;
	float mToeLength;
	float mLastModelY;

	Transform mCharacterTrans;
	DebugDraw* mCharacterDraw;
	FastClip mWalkClip;
	Skeleton mSkeleton;
	std::vector<Mesh> mCharacterMeshes;
	std::vector<mat4> mPoseMatrixPalette;
	Pose mCurPose;
	float mPlayTime;

	DebugDraw* mSkeletonDraw;

	Texture* mDiffuseTexture;
	Shader* mStaticShader;
	Shader* mDynamicShader;

	// UI control
	bool mIsShowGround;
	bool mIsShowCharacter;
	bool mIsShowSkeleton;
public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void OnGUI() override;
	void Shutdown();
};

#endif // _H_CHAPTER13_SAMPLE03_

