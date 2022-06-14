/******************************************************************
** @File         : CCDSolver.h
** @Author       : BanMing
** @Date         : 2022/06/14
** @Description  :
*******************************************************************/

#ifndef _H_CCDSOLVER_
#define _H_CCDSOLVER_

#include <vector>
#include "../Math/Transform.h"

class CCDSolver
{
protected:
	std::vector<Transform> mIKChain;
	// make sure the solver doesn`t fall into an infinite loop
	// limiting the number of iterations helps to avoid a potential infinite loop
	unsigned int mNumSteps;
	float mThreshold;
public:
	CCDSolver();
	unsigned int Size();
	void Resize(unsigned int newSize);
	Transform& operator[](unsigned int index);
	Transform GetGlobalTransform(unsigned int index);
	unsigned int GetNumSteps();
	void SetNumSteps(unsigned int numSteps);
	float GetThreshold();
	void SetThreshold(float value);
	// only the position component of the transform is ever used
	bool Solve(const Transform& target);

};



#endif // _H_CCDSOLVER_
