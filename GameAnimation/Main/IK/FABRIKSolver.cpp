/******************************************************************
** @File         : FABRIKSolver.cpp
** @Author       : BanMing
** @Date         : 2022/06/15
** @Description  :
*******************************************************************/

#include "FABRIKSolver.h"
FABRIKSolver::FABRIKSolver()
{
	mNumSteps = 15;
	mThreshold = 0.00001f;
}

void FABRIKSolver::IKChainToWorld()
{
	unsigned int ikSize = mIKChain.size();
	for (size_t i = 0; i < ikSize; i++)
	{
		Transform world = GetGlobalTransform(i);
		mWorldChain[i] = world.position;
		if (i >= 1)
		{
			mLengths[i] = len(mWorldChain[i - 1] - mWorldChain[i]);
		}
	}
	if (ikSize > 0)
	{
		mLengths[0] = 0.0f;
	}
}

void FABRIKSolver::IterateForward(const vec3& base)
{
	int size = Size();
	if (size > 0)
	{
		mWorldChain[0] = base;
	}

	for (int i = 1; i < size; i++)
	{
		vec3 direction = normalized(mWorldChain[i] - mWorldChain[i - 1]);
		vec3 offset = direction * mLengths[i];
		mWorldChain[i] = mWorldChain[i - 1] + offset;
	}
}

void FABRIKSolver::IterateBackward(const vec3& goal)
{
	int size = Size();
	if (size > 0)
	{
		mWorldChain[size - 1] = goal;
	}

	for (int i = size - 2; i >= 0; i--)
	{
		vec3 direction = normalized(mWorldChain[i] - mWorldChain[i + 1]);
		vec3 offset = direction * mLengths[i + 1];
		mWorldChain[i] = mWorldChain[i + 1] + offset;
	}
}

void FABRIKSolver::WorldToIKChain()
{
	unsigned int ikSize = mIKChain.size();
	if (ikSize == 0)
	{
		return;
	}
	for (size_t i = 0; i < ikSize - 1; i++)
	{
		Transform world = GetGlobalTransform(i);
		Transform next = GetGlobalTransform(i + 1);

		vec3 position = world.position;
		quat rotation = world.rotation;
		// Create a vector that points from the current joint to the next one
		vec3 toNext = next.position - position;
		// This is the rotation between the current node and the next node
		toNext = inverse(rotation) * toNext;

		// Construct a vector that points from the world space IK chain of 
		// the next joint to the current position
		vec3 toDesired = mWorldChain[i + 1] - position;
		// This is the rotation between the current node and
		// the desired position of the next node
		toDesired = inverse(rotation) * toDesired;

		quat delta = fromTo(toNext, toDesired);
		mIKChain[i].rotation = delta * mIKChain[i].rotation;

	}
}

unsigned int FABRIKSolver::Size()
{
	return mIKChain.size();
}

void FABRIKSolver::Resize(unsigned int newSize)
{
	mIKChain.resize(newSize);
	mWorldChain.resize(newSize);
	mLengths.resize(newSize);
}

unsigned int FABRIKSolver::GetNumSteps()
{
	return mNumSteps;
}

void FABRIKSolver::SetNumSteps(unsigned int numSteps)
{
	mNumSteps = numSteps;
}

float FABRIKSolver::GetThreshold()
{
	return mThreshold;
}

void FABRIKSolver::SetThreshold(float value)
{
	mThreshold = value;
}

Transform FABRIKSolver::GetLocalTransform(unsigned int index)
{
	return mIKChain[index];
}

void FABRIKSolver::SetLocalTransform(unsigned int index, const Transform& t)
{
	mIKChain[index] = t;
}

Transform FABRIKSolver::GetGlobalTransform(unsigned int index)
{
	unsigned int size = (unsigned int)mIKChain.size();
	Transform world = mIKChain[index];
	for (int i = index - 1; i >= 0; i--)
	{
		world = combine(mIKChain[i], world);
	}
	return world;
}

bool FABRIKSolver::Solve(const Transform& target)
{
	unsigned int size = Size();
	if (size == 0)
	{
		return false;
	}

	unsigned int last = size - 1;
	float thresholdSq = mThreshold * mThreshold;

	IKChainToWorld();

	vec3 goal = target.position;
	vec3 base = mWorldChain[0];
	for (unsigned int i = 0; i < mNumSteps; i++)
	{
		vec3 effector = mWorldChain[last];
		if (lenSq(goal - effector) < thresholdSq)
		{
			WorldToIKChain();
			return true;
		}

		IterateBackward(goal);

		IterateForward(base);

		//WorldToIKChain();//Needed for constraints
		////TODO:constraints
		//IKChainToWorld();//Needed for constraints
	}

	WorldToIKChain();

	vec3 effector = GetGlobalTransform(last).position;
	if (lenSq(goal - effector) < thresholdSq)
	{
		return true;
	}
	return false;
}
