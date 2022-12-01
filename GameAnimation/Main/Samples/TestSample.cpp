#include "TestSample.h"
#include "../Math/Transform.h"
#include "../Math/vec3.h"
#include "../Math/quat.h"
#include "../Math/mat4.h"

void TestVector()
{
	vec3 a(1, 2, 3);
	vec3 b(2, 3, 4);
	float dotValue = dot(a, b);
	vec3 AcrossB = cross(a, b);
	vec3 BcrossA = cross(b, a);
}

void TestQuat()
{
	quat rotation(0.2f, 0.2f, 0.0f, 1.0f);
	vec3 v = vec3(1, 0, 0);
	vec3 test = rotation * v;

	vec3 a = rotation.vector * 2.0f * dot(rotation.vector, v);
	vec3 b = v * (rotation.w * rotation.w - dot(rotation.vector, rotation.vector));
	vec3 c = cross(rotation.vector, v) * 2.0f * rotation.w;
	vec3 res = a + b + c;
	vec3 d = cross(rotation.vector, v);
}

void TestTransform()
{
	vec3 scale(1.0f, 2.0f, 3.0f);
	vec3 position(-10.8f, 0.0f, 5.3f);
	quat rotation(0.2f, 0.2f, 0.0f, 1.0f);
	Transform trans(position, rotation, scale);

	mat4 quatMat4 = quatToMat4(rotation);
	mat4 transMat4 = transformToMat4(trans);

	vec3 test = rotation * vec3(1, 0, 0);
}

void TestSample::Initialize()
{
	TestVector();
	TestQuat();
	TestTransform();
}

void TestSample::Update(float deltaTime)
{
}

void TestSample::Render(float inAspectRatio)
{
}

void TestSample::OnGUI()
{
}

void TestSample::Shutdown()
{
}
