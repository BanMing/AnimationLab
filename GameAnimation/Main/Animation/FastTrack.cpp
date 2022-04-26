#include "FastTrack.h"

template FastTrack<float, 1>;
template FastTrack<vec3, 3>;
template FastTrack<quat, 4>;

template<typename T, int N>
void FastTrack<T, N>::UpdateIndexLookupTable()
{
	int numFrames = (int)this->mFrames.size();
	// A valid track will have at least two frames
	if (numFrames <= 1)
	{
		return;
	}

	// 60 samples per second of animation
	float duration = this->GetEndTime() - this->GetStartTime();
	unsigned int numSamples = 60 + (unsigned int)(duration * 60.0f);
	mSampledFrames.resize(numSamples);

	// find the time of the sample along the track
	for (unsigned int i = 0; i < numSamples; i++)
	{
		float t = (float)i / (float)(numSamples - 1);
		float time = t * duration + this->GetStartTime();

		unsigned int frameIndex = 0;
		for (int j = numFrames - 1; j >= 0; j--)
		{
			if (time >= this->mFrames[j].mTime)
			{
				frameIndex = (unsigned int)j;
				if ((int)frameIndex >= numFrames - 2)
				{
					frameIndex = numFrames - 2;
				}
				break;
			}
		}
		mSampledFrames[i] = frameIndex;
	}
}

template<typename T, int N>
int FastTrack<T, N>::FrameIndex(float time, bool loop)
{
	std::vector<Frame<N>>& frames = this->mFrames;
	unsigned int size = (unsigned int)frames.size();
	// A valid track will have at least two frames
	if (size <= 1)
	{
		return -1;
	}

	// make sure the requested sample time falls between the start and end times of the tracks.
	if (loop)
	{
		float startTime = this->mFrames[0].mTime;
		float endTime = this->mFrames[size - 1].mTime;
		float duration = endTime - startTime;
		time = fmodf(time - startTime, duration);

		if (time < 0)
		{
			time += duration;
		}
		time += startTime;
	}
	else
	{
		// clamp to first or next to last frame
		if (time <= frames[0].mTime)
		{
			return 0;
		}
		if (time >= frames[size - 2].mTime)
		{
			return (int)size - 2;
		}
	}
	// Find the normalized sample time and frame index
	// The frame index is the normalized sample time scaled by the number of samples
	float duration = this->GetEndTime() - this->GetStartTime();
	float t = time / duration;
	unsigned int numSamples = 60 + (unsigned int)(duration * 60.0f);
	unsigned int index = (unsigned int)(t * (float)numSamples);
	if (index >= mSampledFrames.size())
	{
		return -1;
	}
	return (int)mSampledFrames[index];
}

template FastTrack<float, 1> OptimizeTrack(Track<float, 1>& input);
template FastTrack<vec3, 3> OptimizeTrack(Track<vec3, 3>& input);
template FastTrack<quat, 4> OptimizeTrack(Track<quat, 4>& input);

template<typename T, int N>
FastTrack<T, N> OptimizeTrack(Track<T, N>& input)
{
	FastTrack<T, N> result;
	
	result.SetInterpolation(input.GetInterpolation());
	unsigned int size = input.Size();
	result.Resize(size);
	for (unsigned int i = 0; i < size; i++)
	{
		result[i] = input[i];
	}

	result.UpdateIndexLookupTable();

	return result;
}