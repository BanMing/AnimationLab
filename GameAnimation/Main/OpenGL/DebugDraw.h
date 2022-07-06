#ifndef _H_DEBUGLINE_
#define _H_DEBUGLINE_

#include "Shader.h"
#include "Attribute.h"
#include "../Math/vec3.h"
#include "../Math/mat4.h"
#include "../Animation/Pose.h"

#include <vector>
#include "../IK/CCDSolver.h"
#include "../IK/FABRIKSolver.h"


enum class DebugDrawMode
{
	Lines, Loop, Strip, Points
};

class DebugDraw
{
protected:
	std::vector<vec3> mPoints;
	Attribute<vec3>* mAttribs;
	Shader* mShader;
private:
	DebugDraw(const DebugDraw&);
	DebugDraw& operator=(const DebugDraw&);
public:
	DebugDraw();
	DebugDraw(unsigned int size);
	~DebugDraw();

	unsigned int Size();
	void Resize(unsigned int newSize);
	vec3& operator[](unsigned int index);
	void Push(const vec3& v);

	void FromPose(const Pose& pose);
	void LinesFromIKSolver(CCDSolver& solver);
	void PointsFromIKSolver(CCDSolver& solver);
	void LinesFromIKSolver(FABRIKSolver& solver);
	void PointsFromIKSolver(FABRIKSolver& solver);

	void UpdateOpenGLBuffers();
	void Draw(DebugDrawMode mode, const vec3& color, const mat4& mvp);
};

#endif