#include "Crowd.h"

#include "../OpenGL/Uniform.h"
unsigned int Crowd::Size()
{
	return mCurrentPlayTimes.size();
}

void Crowd::Resize(unsigned int size)
{
	if (size > CROWD_MAX_ACTORS)
	{
		size = CROWD_MAX_ACTORS;
	}
	mPositions.resize(size);
	mRotations.resize(size);
	mScales.resize(size, vec3(1, 1, 1));
	mFrames.resize(size);
	mTimes.resize(size);
	mCurrentPlayTimes.resize(size);
	mNextPlayTimes.resize(size);
}

Transform Crowd::GetActor(unsigned int index)
{
	return Transform(mPositions[index], mRotations[index], mScales[index]);
}

void Crowd::SetActor(unsigned int index, const Transform& t)
{
	mPositions[index] = t.position;
	mRotations[index] = t.rotation;
	mScales[index] = t.scale;
}

void Crowd::Update(float deltaTime, FastClip& mClip, unsigned int texWidth)
{
	bool looping = mClip.GetLooping();
	float start = mClip.GetStartTime();
	float end = mClip.GetEndTime();
	float duration = mClip.GetDuration();

	UpdatePlaybackTimes(deltaTime, looping, start, end);
	UpdateFrameIndices(start, duration, texWidth);
	UpdateInterpolationTimes(start, duration, texWidth);
}

void Crowd::SetUniforms(Shader* shader)
{
	Uniform<vec3>::Set(shader->GetUniform("model_pos"), mPositions);
	Uniform<quat>::Set(shader->GetUniform("model_rot"), mRotations);
	Uniform<vec3>::Set(shader->GetUniform("model_scl"), mScales);
	Uniform<ivec2>::Set(shader->GetUniform("frames"), mFrames);
	Uniform<float>::Set(shader->GetUniform("time"), mTimes);
}

float Crowd::AdjustTime(float t, float start, float end, bool looping)
{
	if (looping)
	{
		t = fmod(t - start, end - start);
		if (t < 0.f)
		{
			t += end - start;
		}
		t += start;
	}
	else
	{
		if (t < start)
		{
			t = start;
		}
		if (t > end)
		{
			t = end;
		}
	}
	return t;
}

void Crowd::UpdatePlaybackTimes(float dt, bool looping, float start, float end)
{
	unsigned int size = mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		float time = mCurrentPlayTimes[i] + dt;
		mCurrentPlayTimes[i] = AdjustTime(time, start, end, looping);
		time = mCurrentPlayTimes[i] + dt;
		mNextPlayTimes[i] = AdjustTime(time, start, end, looping);
	}
}

void Crowd::UpdateFrameIndices(float start, float duration, unsigned int texWidth)
{
	// To find the x coordinate of the current frame,
	// normalize the frame time and then multiply the normalized frame time by the size of the texture.
	unsigned int size = mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		float thisNormalizedTime = (mCurrentPlayTimes[i] - start) / duration;
		unsigned int thisFrame = thisNormalizedTime * (texWidth - 1);
		float nextNormalizedTime = (mNextPlayTimes[i] - start) / duration;
		unsigned int nextFrame = nextNormalizedTime * (texWidth - 1);
		mFrames[i].x = thisFrame;
		mFrames[i].y = nextFrame;
	}
}

void Crowd::UpdateInterpolationTimes(float start, float duration, unsigned int texWidth)
{
	unsigned int size = mCurrentPlayTimes.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (mFrames[i].x == mFrames[i].y)
		{
			mTimes[i] = 1.f;
			continue;
		}

		float thisT = (float) mFrames[i].x / (float) (texWidth - 1);
		float thisTime = start + duration * thisT;
		float nextT = (float) mFrames[i].y / (float) (texWidth - 1);
		float nextTime = start + duration * nextT;
		if (nextTime < thisTime)
		{
			nextTime += duration;
		}

		float frameDuration = nextTime - thisTime;
		mTimes[i] = (mCurrentPlayTimes[i] - thisTime) / frameDuration;
	}
}
