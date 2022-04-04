#ifndef _H_TRANSFORMTRACK_
#define _H_TRANSFORMTRACK_

#include "Track.h"
#include "../Math/Transform.h"

class TransformTrack
{
protected:
	// We only track the bone(joint) what we need to save memory
	unsigned int mId;
	VectorTrack mPosition;
	QuaternionTrack mRotation;
	VectorTrack mScale;

public:
	TransformTrack();
	unsigned int GetId();
	void SetId(unsigned int id);
	VectorTrack& GetPositionTrack();
	QuaternionTrack& GetRotationTrack();
	VectorTrack& GetScaleTrack();
	float GetStartTime();
	float GetEndTime();
	bool IsValid();
	Transform Sample(const Transform& ref, float time, bool looping);
};

#endif