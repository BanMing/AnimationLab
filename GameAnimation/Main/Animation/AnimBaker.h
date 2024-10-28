#ifndef _H_ANIMBAKER_
#define _H_ANIMBAKER_
#include "Skeleton.h"
#include "TClip.h"
#include "AnimTexture.h"

void BakeAnimationToTexture(Skeleton& skel, FastClip& clip, AnimTexture& outTex);

#endif // !_H_ANIMBAKER_
