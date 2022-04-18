#ifndef _H_FAST_TRACK_
#define _H_FAST_TRACK_

#include "Track.h"

template<typename T, int N>
class FastTrack :public Track<T, N>
{
protected:
	std::vector<unsigned int> mSampledFrames;
	virtual int FrameIndex(float time, bool looping);
public:
	void UpdateIndexLookupTable();
};

typedef FastTrack<float, 1> FastScalarTrack;
typedef FastTrack<vec3, 3> FastVectorTrack;
typedef FastTrack<quat, 4> FastQuaternionTrack;

template<typename T,int N>
FastTrack<T, N> OptimizeTrack(Track<T, N>& input);

#endif