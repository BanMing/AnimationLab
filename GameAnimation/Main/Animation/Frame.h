#ifndef _H_FRAME_
#define _H_FRAME_

// avoid to use additional memory by templating intger
template<unsigned int N>
class Frame
{
public:
	float mValue[N];
	// both in and out tangents
	float mIn[N];
	float mOut[N];
	float mTime;
};

typedef Frame<1> ScalarFrame;
typedef Frame<3> VectorFrame;
typedef Frame<4> QuaternionFrame;

#endif