#ifndef _H_SKELETON_
#define _H_SKELETON_

#include "Pose.h"
#include "../Math/mat4.h"
#include <vector>
#include <string>

// Shared,you can have many characters,each with a unique animated pose
// but they can all share the same skeleton.
class Skeleton
{
protected:
	Pose mRestPose;
	Pose mBindPose;
	std::vector<mat4> mInvBindPose;
	std::vector<std::string> mJointNames;
protected:
	void UpdateInverseBindPose();
public:
	Skeleton();
	Skeleton(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);

	void Set(const Pose& rest, const Pose& bind, const std::vector<std::string>& names);

	Pose& GetBindPose();
	Pose& GetRestPose();
	std::vector<mat4>& GetInvBindPose();
	void GetInvBindPose(std::vector<DualQuaternion>& pose);
	std::vector<std::string>& GetJointNames();
	std::string& GetJointName(unsigned int index);
};

#endif