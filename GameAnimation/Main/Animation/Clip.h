#ifndef _H_CLIP_
#define _H_CLIP_

#include <vector>
#include <string>
#include "TransformTrack.h"
#include "Pose.h"

class Clip
{
protected:
	// each joint has a transform track to record its transformation
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
	// find transformation of all joints at time,return adjust time
	float Sample(Pose& outPose, float time);
	TransformTrack& operator[](unsigned int index);
	// loops through all the TransformTrack objects and sets the start/end time
	// of the animation clip based on the tracks that make up the clip.
	void RecalculateDuration();
	std::string& GetName();
	void SetName(const std::string& inNewName);
	float GetDuration();
	float GetStartTime();
	float GetEndTime();
	bool GetLooping();
	void SetLooping(bool inLooping);
};

#endif 