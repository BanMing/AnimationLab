#include "Skeleton.h"

Skeleton::Skeleton() {}

Skeleton::Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
	Set(rest, bind, names);
}

void Skeleton::Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names)
{
	mRestPose = rest;
	mBindPose = bind;
	mJointNames = names;
	UpdateInverseBindPose();
}

void Skeleton::UpdateInverseBindPose()
{
	unsigned int size = mBindPose.Size();
	mInvBindPose.resize(size);

	for (unsigned int i = 0; i < size; i++)
	{
		Transform  world = mBindPose.GetGlobalTransform(i);
		mInvBindPose[i] = inverse(transformToMat4(world));
	}
}

Pose& Skeleton::GetBindPose()
{
	return mBindPose;
}

Pose& Skeleton::GetRestPose()
{
	return mRestPose;
}

std::vector<mat4>& Skeleton::GetInvBindPose()
{
	return mInvBindPose;
}

void Skeleton::GetInvBindPose(std::vector<DualQuaternion>& outInvBindPose)
{
	unsigned int size = mInvBindPose.size();
	outInvBindPose.resize(size);

	for (unsigned int i = 0; i < size; i++)
	{
		DualQuaternion world = mBindPose.GetGlobalDualQuaternion(i);
		outInvBindPose[i] = conjugate(world);
	}
}

std::vector<std::string>& Skeleton::GetJointNames()
{
	return mJointNames;
}

std::string& Skeleton::GetJointName(unsigned int idx)
{
	return mJointNames[idx];
}