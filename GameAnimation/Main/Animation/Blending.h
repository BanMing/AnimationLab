#ifndef _H_BLENDING_
#define _H_BLENDING_

#include "Pose.h"
#include "Skeleton.h"
#include "TClip.h"

bool IsInHierarchy(Pose& pose, unsigned int parent, unsigned int search);
void Blend(Pose& output, Pose& a, Pose& b, float t, int blendroot);

// samples the additive clip at time 0 into an output pose.
// output pose is the reference that is used to add two poses together
Pose MakeAdditivePose(Skeleton& skeleton, Clip& clip);

// result pose = input pose + (additive pose - additive base pose)
void Add(Pose& output, Pose& inPose, Pose& addPose, Pose& additiveBasePose, int blendroot);

#endif

