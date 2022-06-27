/******************************************************************
** @File         : Chapter13Sample01.h
** @Author       : BanMing
** @Date         : 2022/06/20
** @Description  :
*******************************************************************/

#ifndef _H_CHAPTER13_SAMPLE01_
#define _H_CHAPTER13_SAMPLE01_
#include "Application.h"
#include "../Math/vec3.h"
#include "../IK/CCDSolver.h"
#include "../OpenGL/DebugDraw.h"

class Chapter13Sample01 :public Application
{
protected:
	const float kPositionMin = 0.0f;
	const float kPositionMax = 8.0f;

	const float kOrientationMin = -90.0f;
	const float kOrientationMax = 90.0f;
	const int kJointNum = 3;
	const float kTargetLineOffset = 0.3f;
protected:
	Transform mTarget;
	CCDSolver mIKSolver;
	DebugDraw* mPointDraw;
	DebugDraw* mLineDraw;
	DebugDraw* mTargetDraw[3];
	float mCamPitch;
	float mCamYaw;
	float mCamDist;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void OnGUI() override;
	void Shutdown();
};

#endif // _H_CHAPTER13_SAMPLE01_
