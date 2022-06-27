/******************************************************************
** @File         : Chapter13Sample02.h
** @Author       : BanMing
** @Date         : 2022/06/27
** @Description  :
*******************************************************************/

#ifndef _H_CHAPTER13_SAMPLE02_
#define _H_CHAPTER13_SAMPLE02_
#include "Application.h"
#include "../Math/Transform.h"
#include "../IK/FABRIKSolver.h"
#include "../OpenGL/DebugDraw.h"

class Chapter13Sample02 :public Application
{
protected:
	const float kPositionMin = 0.0f;
	const float kPositionMax = 8.0f;

	const float kOrientationMin = -90.0f;
	const float kOrientationMax = 90.0f;
	const int kJointNum = 5;
	const float kTargetLineOffset = 0.3f;
protected:
	Transform mTarget;
	FABRIKSolver mIKSolver;
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
#endif // _H_CHAPTER13_SAMPLE02_

