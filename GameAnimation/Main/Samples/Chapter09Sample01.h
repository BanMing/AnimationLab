#ifndef _H_CHAPTER09_SAMPLE01_
#define _H_CHAPTER09_SAMPLE01_

#include "Application.h"
#include "../OpenGL/DebugDraw.h"
#include "../Animation/Clip.h"
#include <vector>

class Chapter09Sample01 :public Application
{
protected:
	DebugDraw* mRestPoseDraw;
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