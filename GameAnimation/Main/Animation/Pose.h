#ifndef _H_POSE_
#define _H_POSE_

#include <vector>
#include "../Math/Transform.h"

class Pose
{
protected:
	// parallel vectors for joint transforms and their parents
	// each tramsform is in local space
	std::vector<Transform> mJoints;
	std::vector<int> mParents;
public:
	Pose();
	Pose(const Pose& p);
	Pose& operator=(const Pose& p);
	Pose(unsigned int numJoints);
	void Resize(unsigned int size);
	unsigned int Size();
	int GetParent(unsigned int index);
	void SetParent(unsigned int index, int parent);
	Transform GetLocalTransform(unsigned int index);
	void SetLocalTransform(unsigned int index, const Transform& transform);
	Transform GetGlobalTransform(unsigned int index);
	Transform operator[](unsigned int index);

	// For OpenGL,joints convert into a linear array of matrices.
	void GetMatrixPalette(std::vector<mat4>& out);

	bool operator==(const Pose& other);
	bool operator!=(const Pose& other);
};

#endif // !_H_POSE_