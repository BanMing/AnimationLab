#ifndef _H_CROWD_
#define _H_CROWD_

#include "../Math/Transform.h"
#include "../Math/quat.h"
#include "../Math/vec2.h"
#include "../Math/vec3.h"
#include "../OpenGL/Shader.h"
#include "TClip.h"

#include <vector>

// Define the maximum number of crowd actors as 80:
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

	// This function will advance the play time for all actors by delta time
	void UpdatePlaybackTimes(float dt, bool looping, float start, float end);

	// This function will convert the current play time into pixel coordinates along the animation texture's x axis
	void UpdateFrameIndices(float start, float duration, unsigned int texWidth);
	
	// This function should find the interpolation time between the current and next animated frames
	void UpdateInterpolationTimes(float start, float duration, unsigned int texWidth);

protected:
	std::vector<vec3> mPositions;
	std::vector<quat> mRotations;
	std::vector<vec3> mScales;

	// The first component is the u texture coordinate of the current animation frame.
	// The second component is the v texture coordinate of the animation frame
	// that would be shown in the next frame.
	// The v texture coordinate is the joint index.
	std::vector<ivec2> mFrames;

	// The current normalized time between the frames of the two poses
	std::vector<float> mTimes;

	// The current animation playback time for each actor in the crowd
	std::vector<float> mCurrentPlayTimes;

	// The estimated next time in the animation
	std::vector<float> mNextPlayTimes;
};

#endif _H_CROWD_