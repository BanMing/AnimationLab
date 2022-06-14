#include "CCDSolver.h"

CCDSolver::CCDSolver()
{
	mNumSteps = 15;
	mThreshold = 0.00001f;
}

unsigned int CCDSolver::Size()
{
	return mIKChain.size();
}

void CCDSolver::Resize(unsigned int newSize)
{
	mIKChain.resize(newSize);
}

Transform& CCDSolver::operator[](unsigned int index)
{
	return mIKChain[index];
}

Transform CCDSolver::GetGlobalTransform(unsigned int index)
{
	unsigned int size = (unsigned int)mIKChain.size();
	Transform world = mIKChain[index];

	for (int i = index - 1; i >= 0; i--)
	{
		world = combine(mIKChain[i], world);
	}

	return world;
}

unsigned int CCDSolver::GetNumSteps()
{
	return mNumSteps;
}

void CCDSolver::SetNumSteps(unsigned int numSteps)
{
	mNumSteps = numSteps;
}

float CCDSolver::GetThreshold()
{
	return mThreshold;
}

void CCDSolver::SetThreshold(float value)
{
	mThreshold = value;
}

bool CCDSolver::Solve(const Transform& target)
{
	unsigned int size = Size();
	if (size == 0)
	{
		return false;
	}

	unsigned int last = size - 1;
	float thresholdSq = mThreshold * mThreshold;
	vec3 goal = target.position;

	for (unsigned int i = 0; i < mNumSteps; i++)
	{
		vec3 effector = GetGlobalTransform(last).position;
		if (lenSq(goal - effector) < thresholdSq)
		{
			return true;
		}

		for (int j = size - 2; j >= 0; j--)
		{
			effector = GetGlobalTransform(last).position;
			Transform world = GetGlobalTransform(j);

			vec3 toEffector = effector - world.position;
			vec3 toGoal = goal - world.position;

			quat effectorToGoal;
			if (lenSq(toGoal)>mThreshold)
			{
				effectorToGoal = fromTo(toEffector, toGoal);
			}

			quat worldRotated = world.rotation * effectorToGoal;
			quat localRotate = worldRotated * inverse(world.rotation);
			mIKChain[j].rotation = localRotate * mIKChain[j].rotation;

			effector = GetGlobalTransform(last).position;
			if (lenSq(goal - effector) < thresholdSq)
			{
				return true;
			}
		}
	}
	return false;
}
