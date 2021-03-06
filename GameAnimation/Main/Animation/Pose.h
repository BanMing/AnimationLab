#ifndef _H_POSE_
#define _H_POSE_

#include <vector>
#include "../Math/Transform.h"
#include "../Math/DualQuaternion.h"

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
	unsigned int Size() const;
	int GetParent(unsigned int index) const;
	void SetParent(unsigned int index, int parent);
	Transform GetLocalTransform(unsigned int index) const;
	void SetLocalTransform(unsigned int index, const Transform& transform);
	Transform GetGlobalTransform(unsigned int index) const;
	Transform operator[](unsigned int index) const;

	// For OpenGL,joints convert into a linear array of matrices.
	void GetMatrixPalette(std::vector<mat4>& out) const;
	void GetDualQuaternionPalette(std::vector<DualQuaternion>& dqs);

	// Get world dual quaternion of a specified joint
	DualQuaternion GetGlobalDualQuaternion(unsigned int i);

	bool operator==(const Pose& other);
	bool operator!=(const Pose& other);
};

#endif // !_H_POSE_