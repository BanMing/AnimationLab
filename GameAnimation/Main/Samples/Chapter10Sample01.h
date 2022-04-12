#ifndef _H_CHAPTER10_SAMPLE01_
#define _H_CHAPTER10_SAMPLE01_

#include "Application.h"
#include "../Animation/Clip.h"
#include "../OpenGL/DebugDraw.h"
#include "../Animation/Skeleton.h"
#include <vector>

class Chapter10Sample01 : public Application
{
protected:
	Skeleton mSkeleton;
	DebugDraw* mRestPoseDraw;
	DebugDraw* mBindPoseDraw;
	DebugDraw* mCurPoseDraw;
	Pose mCurrentPose;
	std::vector<Clip> mClips;
	float mPlaybackTime;
	unsigned int mCurrentClip;

public:
	void Initialize();
	void Update(float inDeltaTime);
	void Render(float inAspectRatio);
	void Shutdown();
};

#endif