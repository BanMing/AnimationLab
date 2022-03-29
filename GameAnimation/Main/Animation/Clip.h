#ifndef _H_CLIP_
#define _H_CLIP_

#include <vector>
#include <string>
#include "TransformTrack.h"
#include "Pose.h"

// each joint has a transform track to record its transformation
class Clip
{
protected:
	std::vector<TransformTrack> mTracks;
	std::string mName;
	float mStartTime;
	float mEndTime;
	bool mLooping;

protected:
	//  find time is within the range of the current clip
	float AdjustTimeToFitRange(float inTime);

public:
	Clip();
	unsigned int GetIdAtIndex(unsigned int index);
	void SetIdAtIndex(unsigned int index, unsigned int id);
	unsigned int Size();
	float Sample(Pose& outPose, float time);
	TransformTrack& operator[](unsigned int index);
};

#endif 