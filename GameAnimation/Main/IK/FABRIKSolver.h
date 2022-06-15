#ifndef _H_FABRIKSOLVER_
#define  _H_FABRIKSOLVER_

#include <vector>
#include "../Math/Transform.h"

class FABRIKSolver
{
protected:
	std::vector<Transform> mIKChain;
	unsigned int mNumSteps;
	float mThreshold;
	std::vector<vec3> mWorldChain;
	std::vector<float> mLengths;
protected:
	void IKChainToWorld();
	void IterateForward(const vec3& goal);
	void IterateBackward(const vec3& base);
	void WorldToIKChain();
public:
	FABRIKSolver();
	unsigned int Size();
	void Resize(unsigned int newSize);
	unsigned int GetNumSteps();
	void SetNumSteps(unsigned int numSteps);
	float GetThreshold();
	void SetThreshold(float value);
};

#endif // _H_FABRIKSOLVER_

