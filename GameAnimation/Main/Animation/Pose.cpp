#include "Pose.h"

Pose::Pose() {}

Pose::Pose(unsigned int numJoints)
{
	Resize(numJoints);
}

Pose::Pose(const Pose& p)
{
	*this = p;
}

Pose& Pose::operator=(const Pose& p)
{
	if (&p == this)
	{
		return *this;
	}

	if (mParents.size() != p.mParents.size())
	{
		mParents.resize(p.mParents.size());
	}

	if (mJoints.size() != p.mJoints.size())
	{
		mJoints.resize(p.mJoints.size());
	}

	if (p.mJoints.size() > 0)
	{
		memcpy(&mJoints[0], &p.mJoints[0], sizeof(Transform) * mJoints.size());
	}

	if (p.mParents.size() > 0)
	{
		memcpy(&mParents[0], &p.mParents[0], sizeof(int) * mParents.size());
	}

	return *this;
}

void Pose::Resize(unsigned int size)
{
	mParents.resize(size);
	mJoints.resize(size);
}

unsigned int Pose::Size() const
{
	return mJoints.size();
}

Transform Pose::GetLocalTransform(unsigned int index) const
{
	return mJoints[index];
}

void Pose::SetLocalTransform(unsigned int index, const Transform& transform)
{
	mJoints[index] = transform;
}

Transform Pose::GetGlobalTransform(unsigned int index) const
{
	Transform result = mJoints[index];
	for (int parent = mParents[index]; parent >= 0; parent = mParents[parent])
	{
		// get current transform of joint in parent space
		result = combine(mJoints[parent], result);
	}
	return result;
}

Transform Pose::operator[](unsigned int index) const
{
	return GetGlobalTransform(index);
}

void Pose::GetMatrixPalette(std::vector<mat4>& out)const
{
	#if 0

	unsigned int size = Size();
	if (out.size() != size)
	{
		out.resize(size);
	}

	for (unsigned int i = 0; i < size; ++i)
	{
		Transform t = GetGlobalTransform(i);
		out[i] = transformToMat4(t);
	}

	#else

	unsigned int size = Size();
	if (out.size() != size)
	{
		out.resize(size);
	}
	int i = 0;
	for (; i < size; i++)
	{
		int parent = mParents[i];
		if (parent > i)
		{
			break;
		}

		mat4 global = transformToMat4(mJoints[i]);
		if (parent >= 0)
		{
			global = out[parent] * global;
		}
		out[i] = global;
	}

	for (; i < size; i++)
	{
		Transform t = GetGlobalTransform(i);
		out[i] = transformToMat4(t);
	}

	#endif
}

void Pose::GetDualQuaternionPalette(std::vector<DualQuaternion>& dqs)
{
	unsigned int size = Size();
	if (dqs.size() != size)
	{
		dqs.resize(size);
	}

	for (unsigned int i = 0; i < size; i++)
	{
		dqs[i] = GetGlobalDualQuaternion(i);
	}
}

int Pose::GetParent(unsigned int index) const
{
	return mParents[index];
}

void Pose::SetParent(unsigned int index, int parent)
{
	mParents[index] = parent;
}

DualQuaternion Pose::GetGlobalDualQuaternion(unsigned int i)
{
	DualQuaternion result = transformToDualQuat(mJoints[i]);

	for (int p = mParents[i]; p > 0; p = mParents[p])
	{
		DualQuaternion parent = transformToDualQuat(mJoints[p]);
		// multiplication is in reverse
		result = result * parent;
	}

	return result;
}

bool Pose::operator==(const Pose& other)
{
	if (mJoints.size() != other.mJoints.size())
	{
		return false;
	}
	if (mParents.size() != other.mParents.size())
	{
		return false;
	}
	unsigned int size = (unsigned int)mJoints.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		Transform thisLocal = mJoints[i];
		Transform otherLocal = other.mJoints[i];

		int thisParent = mParents[i];
		int otherParent = other.mParents[i];

		if (thisParent != otherParent)
		{
			return false;
		}

		if (thisLocal != otherLocal)
		{
			return false;
		}
	}
	return true;
}

bool Pose::operator!=(const Pose& other)
{
	return !(*this == other);
}