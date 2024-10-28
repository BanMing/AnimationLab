#ifndef _H_CROWD_
#define _H_CROWD_

#include <vector>
#include"../Math/vec2.h"
#include "../Math/vec3.h"
#include "../Math/quat.h"
#include "../Math/Transform.h"
#include "TClip.h"
#include "../OpenGL/Shader.h"

//Define the maximum number of crowd actors as 80:
#define CROWD_MAX_ACTORS 80

struct Crowd
{
public:
	unsigned int Size();
	void Resize(unsigned int size);
	Transform GetActor(unsigned int index);
	void SetActor(unsigned int index, const Transform& t);
	void Update(float deltaTime, FastClip& mClip, unsigned int texWidth);
	void SetUniforms(Shader* shader);
protected:
	float AdjustTime(float t, float start, float end, bool looping);
	void UpdatePlaybackTimes(float dt, bool looping, float start, float end);
	void UpdateFrameIndices(float start, float duration, unsigned int texWidth);
	void UpdateInterpolationTimes(float start, float duration, unsigned int texWidth);
protected:
	std::vector<vec3> mPositions;
	std::vector<quat> mRotations;
	std::vector<vec3> mScales;
	std::vector<ivec2> mFrames;
	std::vector<float> mTimes;
	std::vector<float> mCurrentPlayTimes;
	std::vector<float> mNextPlayTimes;
};

#endif _H_CROWD_