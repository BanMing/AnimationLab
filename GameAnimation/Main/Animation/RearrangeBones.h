#ifndef _H_REARRANGEBONES_
#define _H_REARRANGEBONES_

#include <map>
#include "Skeleton.h"
#include "Mesh.h"
#include "TClip.h"

typedef std::map<int, int> BoneMap;

BoneMap RearrangeSkeleton(Skeleton& skeleton);
void RearrangeMesh(Mesh& mesh, BoneMap& boneMap);
void RearrangeClip(Clip& clip, BoneMap& boneMap);
void RearrangeFastclip(FastClip& clip, BoneMap& boneMap);

#endif