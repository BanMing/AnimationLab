#include "Clip.h"

Clip::Clip()
{
	mName = "No name given";
	mStartTime = 0.0f;
	mEndTime = 0.0f;
	mLooping = true;
}

float Clip::Sample(Pose& outPose, float time)
{
	if (GetDuration() == 0.0f)
	{
		return 0.0f;
	}
	time = AdjustTimeToFitRange(time);

	unsigned int size = mTracks.size();
	for (unsigned int i = 0; i < size; i++)
	{
		unsigned int joint = mTracks[i].GetId();//Joint
		Transform local = outPose.GetGlobalTransform(joint);
		Transform animated = mTracks[i].Sample(local, time, mLooping);
		outPose.SetLocalTransform(joint, animated);
	}

	return time;
}

float Clip::AdjustTimeToFitRange(float inTime)
{
	if (mLooping)
	{
		float duration = mEndTime - mStartTime;
		if (duration <= 0)
		{
			return 0.0f;
		}
		inTime = fmodf(inTime - mStartTime, mEndTime - mStartTime);
		if (inTime < 0.0f)
		{
			inTime += mEndTime - mStartTime;
		}
		inTime = inTime + mStartTime;
	}
	else
	{
		if (inTime < mStartTime)
		{
			inTime = mStartTime;
		}
		if (inTime > mEndTime)
		{
			inTime = mEndTime;
		}
	}
	return inTime;
}

void Clip::RecalculateDuration()
{
	mStartTime = (float)INT_MAX;
	mEndTime = 0;

	unsigned int size = mTracks.size();
	for (unsigned int i = 0; i < size; i++)
	{
		mStartTime = mStartTime < mTracks[i].GetStartTime() ? mStartTime : mTracks[i].GetStartTime();
		mEndTime = mEndTime > mTracks[i].GetEndTime() ? mEndTime : mTracks[i].GetEndTime();
	}

	mStartTime = mStartTime == (float)INT_MAX ? 0 : mStartTime;
}

TransformTrack& Clip::operator[](unsigned int jointID)
{
	unsigned int size = mTracks.size();
	for (unsigned int i = 0; i < size; i++)
	{
		if (mTracks[i].GetId() == jointID)
		{
			return mTracks[i];
		}
	}

	TransformTrack newTrack;
	mTracks.push_back(newTrack);
	mTracks[size].SetId(jointID);

	return mTracks[size];
}

std::string& Clip::GetName()
{
	return mName;
}

void Clip::SetName(const std::string& inNewName)
{
	mName = inNewName;
}

unsigned int Clip::GetIdAtIndex(unsigned int index)
{
	return mTracks[index].GetId();
}

void Clip::SetIdAtIndex(unsigned int index, unsigned int id)
{
	return mTracks[index].SetId(id);
}

unsigned int Clip::Size()
{
	return (unsigned int)mTracks.size();
}

float Clip::GetDuration()
{
	return mEndTime - mStartTime;
}

float Clip::GetStartTime()
{
	return mStartTime;
}

float Clip::GetEndTime()
{
	return mEndTime;
}

bool Clip::GetLooping()
{
	return mLooping;
}

void Clip::SetLooping(bool inLooping)
{
	mLooping = inLooping;
}