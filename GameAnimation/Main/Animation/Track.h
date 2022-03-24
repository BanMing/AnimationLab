#ifndef _H_TRACK_
#define _H_TRACK_

#include <vector>
#include "Frame.h"
#include "Interpolation.h"
#include "../Math/vec3.h"
#include "../Math/quat.h"

//the first is the type(which isintended to be float, vec3, quat, and so on)
//the other is thenumber of components that the type contains
template<typename T, int N>
class Track
{
protected:
	std::vector<Frame<N>> mFrames;
	Interpolation mInterpolation;
protected:
	T SampleConstant(float time, bool looping);
	T SampleLinear(float time, bool looping);
	T SampleCubic(float time, bool looping);
	// Will have one specialization for Quat
	T Hermite(float time, const T& point1, const T& slope1, const T& point2, const T& slope2);
	virtual int FrameIndex(float time, bool looping);
	float AdjustTimeToFitTrack(float time, bool looping);
	// Will be specialized
	T Cast(float* value);
public:
	Track();

	void Resize(unsigned int size);
	unsigned int Size();

	Interpolation GetInterpolation();
	void SetInterpolation(Interpolation interpolation);

	T Sample(float time, bool looping);
	Frame<N>& operator[](unsigned int index);

	float GetStartTime();
	float GetEndTime();
};

typedef Track<float, 1> ScalarTrack;
typedef Track<vec3, 3> VectorTrack;
typedef Track<quat, 4> QuaternionTrack;

#endif