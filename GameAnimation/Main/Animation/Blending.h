#ifndef _H_BLENDING_
#define _H_BLENDING_

#include "Pose.h"

bool IsInHierarchy(Pose& pose, unsigned int parent, unsigned int search);
void Blend(Pose& output, Pose& a, Pose& b, float t, int blendroot);

#endif

